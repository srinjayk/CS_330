#include <cfork.h>
#include <page.h>
#include <mmap.h>

/*
	Name Srinjay Kumar
	Roll Number 170722
*/


/* You need to implement cfork_copy_mm which will be called from do_cfork in entry.c. Don't remove copy_os_pts()*/
void cfork_copy_mm(struct exec_context *child, struct exec_context *parent ){
	//Some of the part of this code is taken from context.c 
	//which was provided to us in the assignment package

	void *os_addr;
	u64 vaddr; 
	struct mm_segment *seg;

	child->pgd = os_pfn_alloc(OS_PT_REG);

	os_addr = osmap(child->pgd);
	bzero((char *)os_addr, PAGE_SIZE);
   
	//CODE segment
	seg = &parent->mms[MM_SEG_CODE];
	for(vaddr = seg->start; vaddr < seg->next_free; vaddr += PAGE_SIZE){
		u64 *parent_pte =  get_user_pte(parent, vaddr, 0);
		if(parent_pte){
			u64 pfn = install_ptable((u64) os_addr, seg, vaddr, (*parent_pte & FLAG_MASK) >> PAGE_SHIFT);
			struct pfn_info* info;
			info = get_pfn_info(pfn);
			increment_pfn_info_refcount(info);
			// asm volatile("invlpg (%0);" :: "r"(vaddr) : "memory");
		}
	} 


	//RODATA segment
	seg = &parent->mms[MM_SEG_RODATA];
	for(vaddr = seg->start; vaddr < seg->next_free; vaddr += PAGE_SIZE){
		u64 *parent_pte =  get_user_pte(parent, vaddr, 0);
		if(parent_pte){
			// if(*parent_pte & 0x2){
			// 	*parent_pte = *parent_pte - 0x2;
			// }
			u64 pfn = install_ptable((u64)os_addr, seg, vaddr, (*parent_pte & FLAG_MASK) >> PAGE_SHIFT); 
			// u64 pfn_addr = (u64)osmap(pfn);
			//memcpy((char *)pfn_addr, (char *)(*parent_pte & FLAG_MASK), PAGE_SIZE); 
			struct pfn_info* info;
			info = get_pfn_info(pfn);
			increment_pfn_info_refcount(info);
			// asm volatile("invlpg (%0);" :: "r"(vaddr) : "memory"); 
		}
	} 
   
	//DATA segment
	seg = &parent->mms[MM_SEG_DATA];
	// //printk("-=========================================");
	for(vaddr = seg->start; vaddr < seg->next_free; vaddr += PAGE_SIZE){
		u64 *parent_pte =  get_user_pte(parent, vaddr, 0);
	  	// //printk("Nikal lavde");
		if(parent_pte){
			if(*parent_pte & 0x2){
			   *parent_pte = *parent_pte - 0x2;
			}
			u64 pfn = install_ptable((u64) os_addr, seg, vaddr, (*parent_pte & FLAG_MASK) >> PAGE_SHIFT); 
			// u64 pfn_addr = (u64)osmap(pfn);
			//memcpy((char *)pfn_addr, (char *)(*parent_pte & FLAG_MASK), PAGE_SIZE);
			u64* child_pte = get_user_pte(child,vaddr,0);
			if(*child_pte & 0x2){
			   *child_pte = *child_pte - 0x2;
			}
			struct pfn_info* info;
			info = get_pfn_info(pfn);
			increment_pfn_info_refcount(info); 
			//printk("||||||parent pte =  %x and child_pte = %x ||||| \n",*parent_pte,*child_pte);
			// asm volatile("invlpg (%0);" :: "r"(vaddr) : "memory"); 
		}
	} 
  
	//STACK segment
	seg = &parent->mms[MM_SEG_STACK];
	for(vaddr = seg->end - PAGE_SIZE; vaddr >= seg->next_free; vaddr -= PAGE_SIZE){
		u64 *parent_pte =  get_user_pte(parent, vaddr, 0);
	  
		if(parent_pte){
			u64 pfn = install_ptable((u64)os_addr, seg, vaddr, 0);  //Returns the blank page  
			u64 pfn_addr = (u64)osmap(pfn);
			memcpy((char *)pfn_addr, (char *)(*parent_pte & FLAG_MASK), PAGE_SIZE); 
			// struct pfn_info* info;
			// info = get_pfn_info(pfn);
			// increment_pfn_info_refcount(info);
			// asm volatile("invlpg (%0);" :: "r"(vaddr) : "memory");
		}
	} 

	//vm area
	

	struct vm_area* parent_vm;
	struct vm_area* prev_child_vm = NULL;
	struct vm_area* child_vm;
	parent_vm = parent->vm_area;
	child_vm = child->vm_area;

	while(parent_vm != NULL){
		struct vm_area* new_vmap;
		new_vmap = alloc_vm_area();
		new_vmap->vm_start = parent_vm->vm_start;
		new_vmap->vm_end = parent_vm->vm_end;
		new_vmap->vm_next = parent_vm->vm_next;
		new_vmap->access_flags = parent_vm->access_flags;
		if(child_vm == NULL){
			child->vm_area = new_vmap;
			prev_child_vm = child->vm_area;
		}
		else{
			prev_child_vm->vm_next = new_vmap;
			prev_child_vm = prev_child_vm->vm_next;
		}
		parent_vm = parent_vm->vm_next;
	}

	int parent_size = 0;
	int child_size = 0;

	// parent_vm = parent->vm_area;
	// child_vm = child->vm_area;

	// while(parent_vm != NULL){
	// 	parent_size++;
	// 	parent_vm = parent_vm->vm_next;
	// }

	// while(child_vm != NULL){
	// 	child_size++;
	// 	child_vm = child_vm->vm_next;
	// }

	struct vm_area* current_vm;
	current_vm = child->vm_area;
	while(current_vm != NULL){
		for(u64 itr = current_vm->vm_start; itr < current_vm->vm_end; itr = itr + PAGE_SIZE){
			u64* ptr = get_user_pte(parent,itr,0);
			if(ptr != NULL){
				if(*ptr & 0x2){
					*ptr = *ptr - 0x2;
				}
				u64 pfn = map_physical_page((u64) os_addr, itr, PROT_READ, (*ptr & FLAG_MASK) >> PAGE_SHIFT);
				u64 pfn_addr = (u64)osmap(pfn);
				//memcpy((char *)pfn_addr, (char *)(*ptr & FLAG_MASK), PAGE_SIZE);
				struct pfn_info* info;
				info = get_pfn_info(pfn);
				increment_pfn_info_refcount(info);
				// asm volatile("invlpg (%0);" :: "r"(vaddr) : "memory");
			}
		}
		current_vm = current_vm->vm_next;
	}

	copy_os_pts(parent->pgd, child->pgd);
	return;
}

/* You need to implement cfork_copy_mm which will be called from do_vfork in entry.c.*/
void vfork_copy_mm(struct exec_context *child, struct exec_context *parent ){
	parent->state = WAITING;
  	struct mm_segment *seg;
  	seg = &parent->mms[MM_SEG_STACK];
  	u64 data_size = seg->end - seg->next_free;
  	u64 parent_rbp = parent->regs.rbp;
  	u64 parent_rsp = parent->regs.entry_rsp;
  	struct mm_segment *child_seg;
  	child_seg = &child->mms[MM_SEG_STACK];
  	child_seg->end = seg->end - data_size;
  	child_seg->next_free = seg->next_free - data_size;
  	for(u64 vaddr = seg->end - PAGE_SIZE; vaddr >= seg->next_free; vaddr -= PAGE_SIZE){
		u64 *parent_pte =  get_user_pte(parent, vaddr, 0);
	  	u64 os_addr;
	  	os_addr = osmap(parent->pgd);
	  	//u64 child_os_addr;
	  	//child_os_addr = osmap(child->pgd);
		if(parent_pte){
			u64 pfn = install_ptable((u64)os_addr, seg, vaddr - data_size, 0);  //Returns the blank page  
			u64 pfn_addr = (u64)osmap(pfn);
			memcpy((char *)pfn_addr, (char *)(*parent_pte & FLAG_MASK ), PAGE_SIZE); 
			//struct pfn_info* info;
			//info = get_pfn_info(pfn);
			//increment_pfn_info_refcount(info);
			asm volatile("invlpg (%0);" :: "r"(vaddr) : "memory");
		}
	}  	
  	//////printk("I am here in 1111111111111111111111\n");
	child->regs.entry_rsp = parent_rsp - data_size;
	child->regs.rbp = parent_rbp - data_size;
	return;
	
}

/*You need to implement handle_cow_fault which will be called from do_page_fault 
incase of a copy-on-write fault

* For valid acess. Map the physical page 
 * Return 1
 * 
 * For invalid access,
 * Return -1. 
*/

int handle_cow_fault(struct exec_context *current, u64 cr2){
	//u64 *ptr =  get_user_pte(current, cr2, 0);
	
	struct mm_segment *seg;
	cr2 = cr2 - (cr2 % PAGE_SIZE);
	seg = &current->mms[MM_SEG_DATA];
	for(u64 vaddr = seg->start; vaddr < seg->next_free; vaddr += PAGE_SIZE){
		if(!(seg->access_flags & PROT_WRITE)){
			return -1;
		}
		if((cr2 >=vaddr) && (cr2 < vaddr + PAGE_SIZE )){
			// //printk("I am here.\n");
			u64 *ptr =  get_user_pte(current, vaddr, 0);
			//printk("\n\n()()%x()()\n\n\n\n",*ptr);
			u64 *os_addr;
			os_addr = osmap(current->pgd);
			if(ptr){
				// //printk("----------------------------");
				//*ptr |= 0x2;
				struct pfn_info* info;
				info = get_pfn_info((*ptr & FLAG_MASK) >> PAGE_SHIFT);
				u8 counting = info->refcount;
				//printk("\n\n()()%x()()\n\n\n\n",*ptr);
				if(counting == 1){
					//printk("I am here\n");
					*ptr = *ptr | 0x2;
					asm volatile("invlpg (%0);" :: "r"(vaddr) : "memory");
					// return 1;
				}
				else if(counting > 1){
					//printk("\n\n()()%x()()\n\n\n\n",*ptr);
					//printk(" I ma here\n");
					u64 page = map_physical_page(osmap(current->pgd),vaddr,PROT_WRITE | PROT_READ,0);
					//printk("\n\n()()%x()()\n\n\n\n",*ptr);
					memcpy((char *)osmap(page), (char *)(*ptr & FLAG_MASK), PAGE_SIZE);
					//printk("\n\n()()%x()()\n\n\n\n",*ptr); 
					struct pfn_info* info_new;
					info_new = get_pfn_info(page);
					// increment_pfn_info_refcount(info_new);
					decrement_pfn_info_refcount(info);
					//printk("\n\n\n====|%x,%x|====\n\n\n\n",*ptr,*get_user_pte(get_ctx_by_pid(2),vaddr,0));
					asm volatile("invlpg (%0);" :: "r"(vaddr) : "memory");
					// return 1;
				}
				else{
					//printk("I am here\n");
					*ptr = *ptr | 0x2;
					asm volatile("invlpg (%0);" :: "r"(vaddr) : "memory");
					// return 1;
				}
				// else if(counting < 0){
				// 	return -1;
				// }
				// asm volatile("invlpg (%0);" :: "r"(vaddr) : "memory");
			}
			else{
				return -1;
			}
		}
		// else{
		// 	return -1;
		// }
	  
	} 
	struct vm_area* current_vm;
	current_vm = current->vm_area;
	// if(current->vm_area == NULL){
		////printk("empty\n");
		//return -1;
	// }
	//////printk("=========================\n");
	//asm volatile("invlpg (%0);" :: "r"(cr2) : "memory");

	// aligning the address provided
	cr2 = cr2 - (cr2 % PAGE_SIZE);

	// getting the correct positon of the addressin the vm area
	while(current_vm != NULL){
		if((cr2 >= current_vm->vm_start)&&(cr2 < current_vm->vm_end)){
			break;
		}
		current_vm = current_vm->vm_next;
		if(current_vm == NULL){
			break;
		}
	}
	/*if(current_vm == NULL){
		////printk("I am here-------------\n");
		return -1;
	}*/
	
	// if(current_vm->access_flags & PROT_WRITE){
	// 	////printk("I am here-------------\n");
	// 	u64 *ptr =  get_user_pte(current, cr2, 0);
	// 	if(ptr == NULL){
	// 		////printk("I am NULL\n");
	// 	}
	// 	if(ptr){
	// 		////printk("I am here123456789765432345678\n");
	// 		//*ptr |= 0x2;
	// 		struct pfn_info* info;
	// 		info = get_pfn_info((*ptr & FLAG_MASK) >> PAGE_SHIFT);
	// 		u8 counting = info->refcount;
	// 		////printk("I am here\n");
	// 		if(counting == 1){
	// 			*ptr = *ptr | 0x2;
	// 			return 1;
	// 		}
	// 		else if(counting > 1){
	// 			////printk(" I ma here\n");
	// 			u32 page = map_physical_page(osmap(current->pgd),cr2,current_vm->access_flags,0);
	// 			//memcpy((char *)page, (char *)(*ptr & FLAG_MASK), PAGE_SIZE); 
	// 			struct pfn_info* info_new;
	// 			info_new = get_pfn_info(page);
	// 			//increment_pfn_info_refcount(info_new);
	// 			decrement_pfn_info_refcount(info);
	// 			return 1;
	// 		}
	// 		else{
	// 			return -1;
	// 		}
	// 	}
	// 	return 1;
	// }
	// else{
	// 	return -1;
	// }

	for(u64 vaddr = current_vm->vm_start; vaddr < current_vm->vm_end; vaddr += PAGE_SIZE){
		if(current_vm->access_flags & PROT_WRITE){
			// //printk("I am here-------------\n");
			u64 *ptr =  get_user_pte(current, cr2, 0);
			// if(ptr == NULL){
			// 	////printk("I am NULL\n");
			// }
			if(ptr){
				////printk("I am here123456789765432345678\n");
				//*ptr |= 0x2;
				struct pfn_info* info;
				info = get_pfn_info((*ptr & FLAG_MASK) >> PAGE_SHIFT);
				u8 counting = info->refcount;
				////printk("I am here\n");
				// if there is single element then give the write permission
				if(counting == 1){
					*ptr = *ptr | 0x2;
					asm volatile("invlpg (%0);" :: "r"(vaddr) : "memory");
					return 1;
				}
				else if(counting > 1){
					////printk(" I ma here\n");
					u32 page = map_physical_page(osmap(current->pgd),cr2,current_vm->access_flags,0);
					memcpy((char *)osmap(page), (char *)(*ptr & FLAG_MASK), PAGE_SIZE); 
					struct pfn_info* info_new;
					info_new = get_pfn_info(page);
					//increment_pfn_info_refcount(info_new);
					decrement_pfn_info_refcount(info);
					asm volatile("invlpg (%0);" :: "r"(vaddr) : "memory");
					return 1;
				}
				else if(counting < 0){
					return -1;
				}
			}
			// return 1;
		}
		else{
			return -1;
		}
	}
	
	return 1;
}

/* You need to handle any specific exit case for vfork here, called from do_exit*/
void vfork_exit_handle(struct exec_context *ctx){
	struct exec_context *parent_ctx = get_ctx_by_pid(ctx->ppid);
	// parent_ctx->state = READY;
	//////printk(" I am in vfork exit\n");
	// struct mm_segment *seg;
  	// seg = &ctx->mms[MM_SEG_STACK];
  	// u64 data_size = seg->end - seg->next_free;
  	//u64 parent_rbp = parent->regs.rbp;
  	//u64 parent_rsp = parent->regs.entry_rsp;

  	// The condition for cfork. In this case, this should not run.
  	if(ctx->pgd != parent_ctx->pgd){
  		return;
  	}
  	// The condition for vfork. This should actually run.
  	if(ctx->pgd == parent_ctx->pgd){
  		parent_ctx->state = READY;
		//////printk(" I am in vfork exit\n");
		struct mm_segment *seg;
  		seg = &ctx->mms[MM_SEG_STACK];
  		// calculating the size of the data in the stack
  		u64 data_size = seg->end - seg->next_free;
  		// itearating all the elements in  the child data part
	  	for(u64 vaddr = seg->end - PAGE_SIZE; vaddr >= seg->next_free; vaddr -= PAGE_SIZE){
			u64 *parent_pte =  get_user_pte(ctx, vaddr, 0);
			//////printk("I am here ion jfjdghfhgdfskhjgbkhjfdsghk\n");
		  	u64 os_addr;
			if(parent_pte){
				u64 pfn = (*parent_pte & FLAG_MASK) >> PAGE_SHIFT;  //Returns the blank page  
				//////printk("I am here ion jfjdghfhgdfskhjg8586586bkhjfdsghk\n");
				//struct pfn_info* info;
				//info = get_pfn_info(pfn);
				os_pfn_free(USER_REG, pfn);
				*parent_pte = NULL;
				asm volatile("invlpg (%0);" :: "r"(vaddr) : "memory");
			}
		}	
		parent_ctx->vm_area = ctx->vm_area;
	}
  	return;
}
