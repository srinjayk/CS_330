#include<types.h>
#include<mmap.h>
#include<page.h>

/*
	Name Srinjay Kumar
	Roll Number 170722
*/

//////////////////////////////////////////////
/////////////////////////////////////////////
/////////////SUBMITTED/////////////////////
//////////////////////////////////////////////
/////////////////////////////////////////////


//////////////////////////////////////////////
/////////////////////////////////////////////
/////////////SUBMITTED/////////////////////
//////////////////////////////////////////////
/////////////////////////////////////////////

//////////////////////////////////////////////
/////////////////////////////////////////////
/////////////SUBMITTED/////////////////////
//////////////////////////////////////////////
/////////////////////////////////////////////

/**
 * Function will invoked whenever there is page fault. (Lazy allocation)
 * 
 * For valid acess. Map the physical page 
 * Return 1
 * 
 * For invalid access,
 * Return -1. 
 */


int vm_area_pagefault(struct exec_context *current, u64 addr, int error_code)
{
	int fault_fixed = -1;

	//printk("I am here in page fault\n");

	if((addr % PAGE_SIZE) != 0){
		//printk("I am here err2\n");
		return fault_fixed;
	}

	if(addr > MMAP_AREA_END){
		return -1;
	}

	if(addr < MMAP_AREA_START){
		return -1;
	}

	struct vm_area* current_vm;
	current_vm = current->vm_area;

	if(error_code == 4){
		//printk("It is read fault\n");
		while(current_vm != NULL){
			if((addr >= current_vm->vm_start)&&(addr < current_vm->vm_end)){
				break;
			}
			current_vm = current_vm->vm_next;
		}
		if(current_vm->access_flags & PROT_WRITE){
			u32 page = map_physical_page(osmap(current->pgd), addr, PROT_READ,0);
			return 1;
		}
		else{
			return -1;
		}	
	}
	else if(error_code == 6){
		//printk("It is a write fault\n");
		while(current_vm != NULL){
			//printk("%x:addr,,,, %x:start,,, %x:end---1\n",addr,current_vm->vm_start,current_vm->vm_end);
			if((addr >= current_vm->vm_start)&&(addr < current_vm->vm_end)){
				//printk("%x:addr,,,, %x:start,,, %x:end---2\n",addr,current_vm->vm_start,current_vm->vm_end);
				break;
			}
			current_vm = current_vm->vm_next;
		}
		if(current_vm->access_flags & PROT_WRITE){
			u32 page = map_physical_page(osmap(current->pgd), addr, PROT_WRITE | PROT_READ,0);
			return 1;
		}
		else{
			return -1;
		}
	}
	else {
		return -1;
	}
	return fault_fixed;
}

int page_calc(struct exec_context *current, u64 addr, int length, int prot){
	int isValid = -1;
	int increment = 0;

	//printk("I am here 40\n");
	struct vm_area* new_vmap_area;
	struct vm_area* curr_vmap_area;
	struct vm_area* prev_vmap_area;

	curr_vmap_area = current->vm_area;
	prev_vmap_area = NULL;

	while(curr_vmap_area){
		//printk("I am here 44\n");
		if((curr_vmap_area->vm_end > addr)&&(curr_vmap_area->vm_start <= addr)){
			break;
		}
		//printk("I am here 49\n");
		curr_vmap_area = curr_vmap_area->vm_next;
	}

	
	struct vm_area* start_vmap_area;
	start_vmap_area = curr_vmap_area;
	curr_vmap_area = current->vm_area;

	while(curr_vmap_area){
		//printk("I am here 44_2\n");
		if((curr_vmap_area->vm_end > addr+length)&&(curr_vmap_area->vm_start <= addr+length)){
			break;
		}
		//printk("I am here 49_2\n");
		curr_vmap_area = curr_vmap_area->vm_next;
	}

	struct vm_area* end_vmap_area;
	end_vmap_area = curr_vmap_area;
	//printk("%x : end_start,,, %x : end_end\n",end_vmap_area->vm_start,end_vmap_area->vm_end);

	prev_vmap_area = start_vmap_area;
	if(prev_vmap_area->vm_next != NULL){
		curr_vmap_area = prev_vmap_area->vm_next;
	}
	else{
		curr_vmap_area = prev_vmap_area;
	}	


	prev_vmap_area = NULL;
	curr_vmap_area = current->vm_area;


	while(1){
		//printk("\n");
		int flag_2 = 0;
		if((curr_vmap_area->vm_start >= addr) && (curr_vmap_area->vm_end <= addr+length)){
			//printk("I am here 41_1\n");
			if(prev_vmap_area == NULL){
				// curr_vmap_area->access_flags = prot;
			}
			else{
				//printk("I am here 41_1_1\n");
				if(prev_vmap_area->access_flags == prot){
					increment--;
					flag_2 = 1;
				}
			}
		}
		else if((curr_vmap_area->vm_start < addr+length) && (curr_vmap_area->vm_start >= addr)){
			//printk("I am here 41_3\n");
			if(prev_vmap_area == NULL){
				if(curr_vmap_area->access_flags != prot){
					increment++;
				}
			}
			else{
				if(curr_vmap_area->access_flags != prot){
					if(prev_vmap_area->access_flags != prot){
						increment++;
					}
					
				}
			}
		}
		else if((curr_vmap_area->vm_end > addr) && (curr_vmap_area->vm_end <= addr+length)){
			//printk("I am here 41_5\n");
			if(curr_vmap_area->access_flags != prot){
				increment++;
			}
		}
		else if((curr_vmap_area->vm_start < addr) && (curr_vmap_area->vm_end > addr+length)){
			//printk("I am here 41_6\n");
			if(prot != curr_vmap_area->access_flags){
				increment = increment + 2;
			}
		}
		else{
			//printk("I am here 41_8\n");
		}
		if(curr_vmap_area == end_vmap_area){
			//printk("I am here 51\n");
			break;
		}
		// if(flag_2){
		// 	struct vm_area* replace;
		// 	replace = curr_vmap_area;
		// 	// dealloc_vm_area(curr_vmap_area);
		// 	curr_vmap_area = prev_vmap_area->vm_next;
		// }
		// else{
		// 	if(prev_vmap_area){
		// 		prev_vmap_area = prev_vmap_area->vm_next;
		// 		curr_vmap_area = curr_vmap_area->vm_next;
		// 	}
		// 	else{
		// 		prev_vmap_area = curr_vmap_area;
		// 		curr_vmap_area = curr_vmap_area->vm_next;
		// 	}
		// }
		prev_vmap_area = curr_vmap_area;
		curr_vmap_area = curr_vmap_area->vm_next;
	}
	return increment;
}
/**
 * mprotect System call Implementation.
 */
int vm_area_mprotect(struct exec_context *current, u64 addr, int length, int prot)
{
	int isValid = -1;

	//printk("I am here 40\n");
	struct vm_area* new_vmap_area;
	struct vm_area* curr_vmap_area;
	struct vm_area* prev_vmap_area;

	curr_vmap_area = current->vm_area;

	if(curr_vmap_area == NULL){
		//printk("I am here 45\n");
		return isValid;
	}

	if((addr % PAGE_SIZE) != 0){
		//printk("I am here err2\n");
		return isValid;
	}

	prev_vmap_area = NULL;

	while(curr_vmap_area){
		//printk("I am here 44\n");
		if((curr_vmap_area->vm_end > addr)&&(curr_vmap_area->vm_start <= addr)){
			break;
		}
		//printk("I am here 49\n");
		curr_vmap_area = curr_vmap_area->vm_next;
	}

	/*if(addr+length > curr_vmap_area->vm_end){
		//printk("I am here 70\n");
		return isValid;
	}*/
	struct vm_area* start_vmap_area;
	start_vmap_area = curr_vmap_area;
	curr_vmap_area = current->vm_area;

	while(curr_vmap_area){
		//printk("I am here 44_2\n");
		if((curr_vmap_area->vm_end > addr+length)&&(curr_vmap_area->vm_start <= addr+length)){
			break;
		}
		//printk("I am here 49_2\n");
		curr_vmap_area = curr_vmap_area->vm_next;
	}

	if(addr+length > curr_vmap_area->vm_end){
		//printk("I am here 70\n");
		return isValid;
	}

	struct vm_area* end_vmap_area;
	end_vmap_area = curr_vmap_area;
	//printk("%x : end_start,,, %x : end_end\n",end_vmap_area->vm_start,end_vmap_area->vm_end);

	prev_vmap_area = start_vmap_area;
	if(prev_vmap_area->vm_next != NULL){
		curr_vmap_area = prev_vmap_area->vm_next;
	}
	else{
		curr_vmap_area = prev_vmap_area;
	}
	
	if(curr_vmap_area != prev_vmap_area){
		if(prev_vmap_area->vm_end != curr_vmap_area->vm_start || curr_vmap_area == NULL){
			//printk("I am here 48\n");
			return isValid;
		}
	}	

	prev_vmap_area = start_vmap_area;
	curr_vmap_area = prev_vmap_area->vm_next;
	while(curr_vmap_area != end_vmap_area && curr_vmap_area){
		//printk("I am here 42\n");
		if(prev_vmap_area){
			if(prev_vmap_area->vm_end != curr_vmap_area->vm_start){
				//printk("I am here 43\n");
				return isValid;
			}
		}
		prev_vmap_area = curr_vmap_area;
		curr_vmap_area = curr_vmap_area->vm_next;
	}

	prev_vmap_area = NULL;
	curr_vmap_area = current->vm_area;

	int increment = page_calc(current,addr,length,prot);
	if(increment + stats->num_vm_area > 128){
		return -1;
	}

	for(u64 itr = addr; itr < addr + length; itr = itr + PAGE_SIZE){
		u64* ptr = get_user_pte(current,itr,0);
		if(ptr != NULL){
			*ptr = *ptr|0x5;
			if(prot & MM_WR){
				*ptr|= 0x2;
			}
			else{
				if(*ptr & 0x2){
					*ptr = *ptr - 0x2;
				}
			}
		}
		//flush tlb
		asm volatile("invlpg (%0);" :: "r"(itr) : "memory");
	}

	prev_vmap_area = NULL;
	curr_vmap_area = current->vm_area;

	while(1){
		//printk("\n");
		int flag_2 = 0;
		if((curr_vmap_area->vm_start >= addr) && (curr_vmap_area->vm_end <= addr+length)){
			//printk("I am here 41_1\n");
			if(prev_vmap_area == NULL){
				curr_vmap_area->access_flags = prot;
			}
			else{
				//printk("I am here 41_1_1\n");
				if(prev_vmap_area->access_flags == prot){
					//printk("I am here 41_1_2\n");
					prev_vmap_area->vm_end = curr_vmap_area->vm_end;
					prev_vmap_area->vm_next = curr_vmap_area->vm_next;
					//dealloc_vm_area(curr_vmap_area);
					flag_2 = 1;
				}
			}
		}
		else if((curr_vmap_area->vm_start < addr+length) && (curr_vmap_area->vm_start >= addr)){
			//printk("I am here 41_3\n");
			if(prev_vmap_area == NULL){
				if(curr_vmap_area->access_flags != prot){
					struct vm_area* create_1;
					create_1 = alloc_vm_area();
					create_1->vm_start = addr+length;
					create_1->vm_end = curr_vmap_area->vm_end;
					create_1->access_flags = curr_vmap_area->access_flags;
					create_1->vm_next = curr_vmap_area->vm_next;
					curr_vmap_area->vm_end = addr+length;
					curr_vmap_area->vm_next = create_1;
					curr_vmap_area->access_flags = prot;
				}
			}
			else{
				if(curr_vmap_area->access_flags != prot){
					if(prev_vmap_area->access_flags == prot){
						prev_vmap_area->vm_end = addr + length;
						curr_vmap_area->vm_start = addr + length;
					}
					else{
						struct vm_area* create_1;
						create_1 = alloc_vm_area();
						create_1->vm_start = addr+length;
						create_1->vm_end = curr_vmap_area->vm_end;
						create_1->access_flags = curr_vmap_area->access_flags;
						create_1->vm_next = curr_vmap_area->vm_next;
						curr_vmap_area->vm_end = addr+length;
						curr_vmap_area->vm_next = create_1;
						curr_vmap_area->access_flags = prot;
					}
					
				}
			}
		}
		else if((curr_vmap_area->vm_end > addr) && (curr_vmap_area->vm_end <= addr+length)){
			//printk("I am here 41_5\n");
			if(curr_vmap_area->access_flags != prot){
				struct vm_area* create_1;
				create_1 = alloc_vm_area();
				create_1->vm_start = addr;
				create_1->vm_end = curr_vmap_area->vm_end;
				create_1->access_flags = prot;
				create_1->vm_next = curr_vmap_area->vm_next;
				curr_vmap_area->vm_end = addr;
				curr_vmap_area->vm_next = create_1;
			}
		}
		else if((curr_vmap_area->vm_start < addr) && (curr_vmap_area->vm_end > addr+length)){
			//printk("I am here 41_6\n");
			if(prot != curr_vmap_area->access_flags){
				struct vm_area* create_1;
				struct vm_area* create_2;
				create_1 = alloc_vm_area();
				create_2 = alloc_vm_area();
				create_1->vm_start = addr;
				create_1->vm_end = addr+length;
				create_1->access_flags = prot;
				create_1->vm_next = NULL;
				create_2->vm_start = addr+length;
				create_2->vm_end = curr_vmap_area->vm_end;
				create_2->access_flags = curr_vmap_area->access_flags;
				create_2->vm_next = NULL;
				curr_vmap_area->vm_end = addr;
				create_2->vm_next = curr_vmap_area->vm_next;
				curr_vmap_area->vm_next = create_1;
				create_1->vm_next = create_2;
			}
		}
		else{
			//printk("I am here 41_8\n");
		}
		if(curr_vmap_area == end_vmap_area){
			//printk("I am here 51\n");
			break;
		}
		if(flag_2){
			struct vm_area* replace;
			replace = curr_vmap_area;
			dealloc_vm_area(replace);
			curr_vmap_area = prev_vmap_area->vm_next;
		}
		else{
			if(prev_vmap_area){
				prev_vmap_area = prev_vmap_area->vm_next;
				curr_vmap_area = curr_vmap_area->vm_next;
			}
			else{
				prev_vmap_area = curr_vmap_area;
				curr_vmap_area = curr_vmap_area->vm_next;
			}
		}
	}

	return 0;
}

/**
 * mmap system call implementation.
 */
long vm_area_map(struct exec_context *current, u64 addr, int length, int prot, int flags)
{

	long ret_addr = -1;

	if(length < 0){
		//printk("I am here err1\n");
		return ret_addr;
	}

	if((addr % PAGE_SIZE) != 0){
		//printk("%x I am here errsdfgh2\n",PAGE_SIZE);
		return ret_addr;
	}

	if(length%PAGE_SIZE != 0){
		length = ((length/PAGE_SIZE) + 1)*PAGE_SIZE;
	}

	int flags1 = -1;

	if(addr != NULL){
		//printk("I am here 21\n");
		struct vm_area* new_vmap_area; 
		struct vm_area* next_curr_vm_area;
		struct vm_area* current_vm;
		current_vm = current->vm_area;
		next_curr_vm_area = current_vm->vm_next;

		if(addr % PAGE_SIZE){
			return -1;
		}

		if(!(current_vm)){
			//printk("*****************1\n");
			if(stats->num_vm_area > 127){
				return -1;
			}
			new_vmap_area = alloc_vm_area();
			new_vmap_area->vm_start = addr;
			new_vmap_area->vm_end = addr + length;
			new_vmap_area->access_flags = prot;
			new_vmap_area->vm_next = NULL;
			current->vm_area = new_vmap_area;
			return new_vmap_area->vm_start;
		}
		else{
			//printk("*****************2\n");
			if(addr + length < current_vm->vm_start){
				//printk("*****************3\n");
				if(stats->num_vm_area > 127){
					return -1;
				}
				new_vmap_area = alloc_vm_area();
				new_vmap_area->vm_start = addr;
				new_vmap_area->vm_end = addr + length;
				new_vmap_area->access_flags = prot;
				new_vmap_area->vm_next = current_vm;
				current->vm_area = new_vmap_area;
				if(flags & MAP_POPULATE){
					for(u64 itr = new_vmap_area->vm_start; itr < new_vmap_area->vm_start+length; itr = itr + PAGE_SIZE){
						u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
					}
				}
				return new_vmap_area->vm_start;
			}
			else if(addr + length == current_vm->vm_start){
				//printk("*****************3\n");
				if(prot == current_vm->access_flags){
					current_vm->vm_start = current_vm->vm_start - length;
					if(flags & MAP_POPULATE){
						for(u64 itr = current_vm->vm_start; itr < current_vm->vm_start+length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return current_vm->vm_start;
				}
				else{
					if(stats->num_vm_area > 127){
						return -1;
					}
					new_vmap_area = alloc_vm_area();
					new_vmap_area->vm_start = addr;
					new_vmap_area->vm_end = addr + length;
					new_vmap_area->access_flags = prot;
					new_vmap_area->vm_next = current_vm;
					current->vm_area = new_vmap_area;
					if(flags & MAP_POPULATE){
						for(u64 itr = new_vmap_area->vm_start; itr < new_vmap_area->vm_start+length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return new_vmap_area->vm_start;
				}
			}
		}

		while(current_vm && next_curr_vm_area){
			//printk("*****************5\n");
			//printk("%x:curr,,%x:addr,,%x:next\n",current_vm->vm_end,addr,next_curr_vm_area->vm_start);
			if((current_vm->vm_end <= addr)&&(next_curr_vm_area->vm_start >= addr + length)||
				(current_vm->vm_end >= addr)){
				//printk("*****************6\n");
				break;
			}
			current_vm = current_vm->vm_next;
			next_curr_vm_area = next_curr_vm_area->vm_next;
		}
		next_curr_vm_area = current_vm->vm_next;
		if(next_curr_vm_area == NULL){
			if(addr+length > MMAP_AREA_END){
				//printk("I am here 23\n");
				flags1 = 1;
				if(flags & MAP_FIXED){
					//printk("I am here 27\n");
					return -1;
				}
			}
		}
		else{
			if(length+addr > next_curr_vm_area->vm_start){
				flags1 = 1;
				//printk("I am here 24\n");
				if(flags & MAP_FIXED){
					//printk("I am here 28\n");
					return -1;
				}
			}
			if(addr == current_vm->vm_start){
				flags1 = 1;
				//printk("I am here 24_1\n");
				if(flags & MAP_FIXED){
					//printk("I am here 28\n");
					return -1;
				}
			}
			if(addr+length == current_vm->vm_end){
				flags1 = 1;
				//printk("I am here 24\n");
				if(flags & MAP_FIXED){
					//printk("I am here 28\n");
					return -1;
				}
			}
		}

		//printk("%d\n",flags1);
		// if(next_curr_vm_area == NULL){
		// 	//printk("I am the last element\n");
		// }

		struct vm_area* current_vm_1 = current->vm_area;
		while(current_vm_1){
			if((current_vm_1->vm_start <= addr)&&(current_vm_1->vm_end > addr)){
				flags1 = 1;
			}
			current_vm_1 = current_vm_1->vm_next;
		}

		////printk("%d\n",flags1);
		if(next_curr_vm_area == NULL && flags1 != 1){
			// printk("I am the last element\n");
			if(current_vm->access_flags == prot){
				if(current_vm->vm_end == addr){
					current_vm->vm_end = addr + length;
					if(flags & MAP_POPULATE){
						for(u64 itr = addr; itr < addr+length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return addr;
				}
				else{
					if(stats->num_vm_area > 127){
						return -1;
					}
					new_vmap_area = alloc_vm_area();
					new_vmap_area->vm_end = addr + length;
					new_vmap_area->vm_start = addr;
					new_vmap_area->vm_next =  NULL;
					new_vmap_area->access_flags = prot;
					current_vm->vm_next = new_vmap_area;
					if(flags & MAP_POPULATE){
						for(u64 itr = addr; itr < addr+length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return addr;
				}
			}
			else{
				if(stats->num_vm_area > 127){
					return -1;
				}
				new_vmap_area = alloc_vm_area();
				new_vmap_area->vm_end = addr + length;
				new_vmap_area->vm_start = addr;
				new_vmap_area->vm_next =  NULL;
				new_vmap_area->access_flags = prot;
				current_vm->vm_next = new_vmap_area;
				if(flags & MAP_POPULATE){
					for(u64 itr = addr; itr < addr+length; itr = itr + PAGE_SIZE){
						u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
					}
				}
				return addr;
			}
		}
		
		//printk("%d : flags\n",flags);
		if(flags1 != 1){
			//printk("*****************4\n");
			//printk("I am here 20\n");
			if((current_vm->vm_end == addr) && (next_curr_vm_area->vm_start == addr+length)){
				// Case 5
				//printk("I am here 21_1\n");
				if((next_curr_vm_area->access_flags == prot)&&(current_vm->access_flags == prot)){
					int add_length = (next_curr_vm_area->vm_end - next_curr_vm_area->vm_start);
					current_vm->vm_end = current_vm->vm_end + add_length + length;
					current_vm->vm_next = next_curr_vm_area->vm_next;
					dealloc_vm_area(next_curr_vm_area);
					if(flags & MAP_POPULATE){
						for(u64 itr = addr; itr < addr+length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return addr;
				}
				// Case 6
				else if(next_curr_vm_area->access_flags == prot){
					next_curr_vm_area->vm_start = next_curr_vm_area->vm_start - length;
					if(flags & MAP_POPULATE){
						for(u64 itr = addr; itr < addr+length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return addr;
				}
				// Case 7
				else if(current_vm->access_flags == prot){
					current_vm->vm_start = current_vm->vm_start + length;
					if(flags & MAP_POPULATE){
						for(u64 itr = addr; itr < addr+length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return addr;
				}
				// Case 8
				else{
					if(stats->num_vm_area > 127){
						return -1;
					}
					new_vmap_area = alloc_vm_area();
					new_vmap_area->vm_start = current_vm->vm_end;
					new_vmap_area->vm_end = current_vm->vm_end + length;
					new_vmap_area->access_flags = prot;
					new_vmap_area->vm_next = next_curr_vm_area;
					current_vm->vm_next = new_vmap_area;
					return new_vmap_area->vm_start;
				}
			}
			else if((current_vm->vm_end == addr)&&(next_curr_vm_area->vm_start < addr+length)){
				//printk("I am here 21_2\n");
				if(current_vm->access_flags == prot){
					current_vm->vm_end = current_vm->vm_end + length;
					if(flags & MAP_POPULATE){
						for(u64 itr = addr; itr < addr+length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return addr;
				}
				// Case 8
				else{
					if(stats->num_vm_area > 127){
						return -1;
					}
					new_vmap_area = alloc_vm_area();
					new_vmap_area->vm_start = addr;
					new_vmap_area->vm_end = addr + length;
					new_vmap_area->access_flags = prot;
					new_vmap_area->vm_next = next_curr_vm_area;
					current_vm->vm_next = new_vmap_area;
					if(flags & MAP_POPULATE){
						for(u64 itr = addr; itr < addr+length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return new_vmap_area->vm_start;
				}
			}
			else if((current_vm->vm_end < addr)&&(next_curr_vm_area->vm_start == addr+length)){
				//printk("I am here 21_3\n");
				// Case 6
				if(next_curr_vm_area->access_flags == prot){
					next_curr_vm_area->vm_start = next_curr_vm_area->vm_start - length;
					if(flags & MAP_POPULATE){
						for(u64 itr = addr; itr < addr+length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return next_curr_vm_area->vm_start;
				}
				// Case 8
				else{
					if(stats->num_vm_area > 127){
						return -1;
					}
					new_vmap_area = alloc_vm_area();
					new_vmap_area->vm_start = addr;
					new_vmap_area->vm_end = addr + length;
					new_vmap_area->access_flags = prot;
					new_vmap_area->vm_next = next_curr_vm_area;
					current_vm->vm_next = new_vmap_area;
					if(flags & MAP_POPULATE){
						for(u64 itr = addr; itr < addr+length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return new_vmap_area->vm_start;
				}
			}
			else{
				if(stats->num_vm_area > 127){
					return -1;
				}
				//printk("I am here 21_4\n");
				new_vmap_area = alloc_vm_area();
				new_vmap_area->vm_start = addr;
				new_vmap_area->vm_end = addr + length;
				new_vmap_area->access_flags = prot;
				new_vmap_area->vm_next = next_curr_vm_area;
				current_vm->vm_next = new_vmap_area;
				current_vm = current->vm_area;
				if(flags & MAP_POPULATE){
					for(u64 itr = addr; itr < addr+length; itr = itr + PAGE_SIZE){
						u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
					}
				}
				return new_vmap_area->vm_start;
			}
		}
	}
	if(addr == NULL || flags1 == 1){
		//printk("I am here 2\n");
		if(length > (MMAP_AREA_END-MMAP_AREA_START)){
			return ret_addr;
		}

		struct vm_area* current_vm;
		current_vm = current->vm_area;

		// Case 1
		// The entire list is empty
		if(current->vm_area == NULL){
			//printk("I am here 1 %d\n",length);
			if(stats->num_vm_area > 127){
				return -1;
			}
			struct vm_area* new_vmap_area; 
			new_vmap_area = alloc_vm_area();
			new_vmap_area->vm_start = MMAP_AREA_START;
			new_vmap_area->vm_end = MMAP_AREA_START + length;
			new_vmap_area->access_flags = prot;
			new_vmap_area->vm_next = NULL;
			current->vm_area = new_vmap_area;
			if(flags & MAP_POPULATE){
				for(u64 itr = new_vmap_area->vm_start; itr < new_vmap_area->vm_start+length; itr = itr + PAGE_SIZE){
					u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
				}
			}
			return new_vmap_area->vm_start;
		}
		// Case 2
		// If there is area present in the start but the entire mapping is not empty
		else if( (current->vm_area->vm_start - MMAP_AREA_START) > length){
			//printk("I am here 3\n");
			if(stats->num_vm_area > 127){
				return -1;
			}
			struct vm_area* new_vmap_area; 
			new_vmap_area = alloc_vm_area();
			new_vmap_area->vm_start = MMAP_AREA_START;
			new_vmap_area->vm_end = MMAP_AREA_START + length;
			new_vmap_area->access_flags = prot;
			new_vmap_area->vm_next = current->vm_area;
			//new_vmap_area->vm_next = current_vm;
			current->vm_area = new_vmap_area;
			if(flags & MAP_POPULATE){
				for(u64 itr = new_vmap_area->vm_start; itr < new_vmap_area->vm_start+length; itr = itr + PAGE_SIZE){
					u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
				}
			}
			return new_vmap_area->vm_start;
		}
		// Case 3 and Case 4
		// The area required will be merging with the area at the head if both of them touch
		else if( (current->vm_area->vm_start - MMAP_AREA_START) == length){
			//printk("%d \n",(current->vm_area->vm_start - MMAP_AREA_START));
			//printk("I  am here 6\n");
			if(stats->num_vm_area > 127){
				//return -1;
				if(current_vm->access_flags != current_vm->vm_next->access_flags){
					return -1;
				}
			}
			struct vm_area* new_vmap_area; 
			new_vmap_area = alloc_vm_area();
			new_vmap_area->vm_start = MMAP_AREA_START;
			new_vmap_area->vm_end = MMAP_AREA_START + length;
			new_vmap_area->access_flags = prot;
			new_vmap_area->vm_next = current->vm_area;
			// We merge both the if the potection permissions are same
			current->vm_area = new_vmap_area;
			if(current_vm->access_flags == current_vm->vm_next->access_flags){
				current_vm->vm_end = current_vm->vm_next->vm_end;
				current_vm->vm_next = current_vm->vm_next->vm_next;
				dealloc_vm_area(current_vm->vm_next);
			}
			if(flags & MAP_POPULATE){
				for(u64 itr = new_vmap_area->vm_start; itr < new_vmap_area->vm_start+length; itr = itr + PAGE_SIZE){
					u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
				}
			}
			return new_vmap_area->vm_start;
		}
		// If the required area is not present in the beginning
		// Case 5, Case 6, Case 7, Case 8, Case 9
		else{
			//printk("I am here 7\n");
			struct vm_area* new_vmap_area; 
			struct vm_area* next_curr_vm_area;
			next_curr_vm_area = current_vm->vm_next;
			// Finding the appropriate part of the memory
			while((next_curr_vm_area->vm_start - current_vm->vm_end) < length){
				current_vm = current_vm->vm_next;
				next_curr_vm_area = next_curr_vm_area->vm_next;
			}
			if(next_curr_vm_area == NULL){
				//printk("It is the last element\n");
			}
			// return if we do not have the required area empty
			if((MMAP_AREA_END - next_curr_vm_area->vm_end) < length){
				return ret_addr;
			}
			// Case 5, Case 6, Case 7, Case 8
			if((next_curr_vm_area->vm_start - current_vm->vm_end) == length){
				// Case 5
				if((next_curr_vm_area->access_flags == prot)&&(current_vm->access_flags == prot)){
					//printk("I am here 7_1\n");
					int add_length = (next_curr_vm_area->vm_end - next_curr_vm_area->vm_start);
					current_vm->vm_end = current_vm->vm_end + add_length + length;
					current_vm->vm_next = next_curr_vm_area->vm_next;
					dealloc_vm_area(next_curr_vm_area);
					if(flags & MAP_POPULATE){
						for(u64 itr = current_vm->vm_end - add_length - length; 
							itr < current_vm->vm_end - add_length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return (current_vm->vm_end - add_length - length);
				}
				// Case 6
				else if(next_curr_vm_area->access_flags == prot){
					//printk("I am here 7_2\n");
					next_curr_vm_area->vm_start = next_curr_vm_area->vm_start - length;
					if(flags & MAP_POPULATE){
						for(u64 itr = next_curr_vm_area->vm_start; 
							itr < next_curr_vm_area->vm_start + length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return next_curr_vm_area->vm_start;
				}
				// Case 7
				else if(current_vm->access_flags == prot){
					//printk("I am here 7_3\n");
					current_vm->vm_end = current_vm->vm_end + length;
					if(flags & MAP_POPULATE){
						for(u64 itr = current_vm->vm_end - length;
							itr < current_vm->vm_end; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return current_vm->vm_end - length;
				}
				// Case 8
				else{
					//printk("I am here 7_4\n");
					if(stats->num_vm_area > 127){
						return -1;
					}
					new_vmap_area = alloc_vm_area();
					new_vmap_area->vm_start = current_vm->vm_end;
					new_vmap_area->vm_end = current_vm->vm_end + length;
					new_vmap_area->access_flags = prot;
					new_vmap_area->vm_next = next_curr_vm_area;
					current_vm->vm_next = new_vmap_area;
					if(flags & MAP_POPULATE){
						for(u64 itr = new_vmap_area->vm_start; itr < new_vmap_area->vm_start+length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return new_vmap_area->vm_start;
				}
			}
			else if(next_curr_vm_area != NULL){
				// Case 9
				if((next_curr_vm_area->access_flags == prot)&&(current_vm->access_flags == prot)){
					//printk("I am here 7_5\n");
					current_vm->vm_end = current_vm->vm_end + length;
					if(flags & MAP_POPULATE){
						for(u64 itr = current_vm->vm_end - length;
							itr < current_vm->vm_end; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return current_vm->vm_end - length;
				}
				// Case 10
				else if(next_curr_vm_area->access_flags == prot){
					//printk("I am here 7_6\n");
					next_curr_vm_area->vm_start = next_curr_vm_area->vm_start - length;
					if(flags & MAP_POPULATE){
						for(u64 itr = next_curr_vm_area->vm_start; 
							itr < next_curr_vm_area->vm_start + length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return next_curr_vm_area->vm_start;
				}
				// Case 11
				else if(current_vm->access_flags == prot){
					//printk("I am here 7_7\n");
					current_vm->vm_start = current_vm->vm_start + length;
					if(flags & MAP_POPULATE){
						for(u64 itr = current_vm->vm_end - length;
							itr < current_vm->vm_end; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return current_vm->vm_end - length;
				}
				// Case 12
				else{
					//printk("I am here 7_8\n");
					if(stats->num_vm_area > 127){
						return -1;
					}
					new_vmap_area = alloc_vm_area();
					new_vmap_area->vm_start = current_vm->vm_end;
					new_vmap_area->vm_end = current_vm->vm_end + length;
					new_vmap_area->access_flags = prot;
					new_vmap_area->vm_next = next_curr_vm_area;
					current_vm->vm_next = new_vmap_area;
					if(flags & MAP_POPULATE){
						for(u64 itr = new_vmap_area->vm_start; itr < new_vmap_area->vm_start+length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return new_vmap_area->vm_start;
				}
			}
			else{
			 	if(current_vm->access_flags == prot){
					//printk("I am here 7_10\n");
					current_vm->vm_end = current_vm->vm_end + length;
					if(flags & MAP_POPULATE){
						for(u64 itr = current_vm->vm_end - length;
							itr < current_vm->vm_end; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return current_vm->vm_end - length;
				}
				// Case 12
				else{
					//printk("I am here 7_11\n");
					if(stats->num_vm_area > 127){
						return -1;
					}
					new_vmap_area = alloc_vm_area();
					new_vmap_area->vm_start = current_vm->vm_end;
					new_vmap_area->vm_end = current_vm->vm_end + length;
					new_vmap_area->access_flags = prot;
					new_vmap_area->vm_next = next_curr_vm_area;
					current_vm->vm_next = new_vmap_area;
					if(flags & MAP_POPULATE){
						for(u64 itr = new_vmap_area->vm_start; itr < new_vmap_area->vm_start+length; itr = itr + PAGE_SIZE){
							u32 page = map_physical_page(osmap(current->pgd), itr , prot, 0);
						}
					}
					return new_vmap_area->vm_start;
				}
			}
		}
	}
	
	return ret_addr;
}


/**
 * munmap system call implemenations
 */

int vm_area_unmap(struct exec_context *current, u64 addr, int length)
{
	int isValid = -1;
	int flag = 2;
	// can change to non initialization
	if(addr == NULL){
		return isValid;
	}

	//printk("I am here 5\n");

	if(length > (MMAP_AREA_END-MMAP_AREA_START)){
			return isValid;
	}

	if(length < 0){
		return isValid;
	}

	if(addr < 0){
		return isValid;
	}

	if((addr % PAGE_SIZE) != 0){
		//printk("I am here err2\n");
		return -1;
	}

	if(length%PAGE_SIZE != 0){
		length = ((length/PAGE_SIZE) + 1)*PAGE_SIZE;
	}

	struct vm_area* buffer_vmap_area; 
	struct vm_area* next_curr_vm_area;
	struct vm_area* current_vm;
	current_vm = current->vm_area;
	buffer_vmap_area = NULL;

	while((current_vm->vm_start >= addr)&&(current_vm->vm_end <= addr+length)){
		//printk("I am in an infinite loop 1\n");
		if(current_vm->vm_next == NULL){
			break;
		}
		buffer_vmap_area = current_vm;
		current_vm = current_vm->vm_next;
	}

	if((current_vm->vm_start <= addr)&&(current_vm->vm_end >= addr+length)){
		flag = 1;
	}
	else{
		flag = 2;
	}

	for(u64 itr = addr; itr < addr+length; itr = itr+PAGE_SIZE){
		//printk("In mmap unmap\n");
		u64* ptr = get_user_pte(current,itr,0);
		if(ptr != NULL){
			struct pfn_info* info_new;
			info_new = get_pfn_info((*ptr & FLAG_MASK) >> PAGE_SHIFT);
			u8 counting = info_new->refcount;
			if(counting == 1){
				os_pfn_free(USER_REG,((*ptr & FLAG_MASK) >> PAGE_SHIFT));
				*ptr = NULL;
			}
			else if(counting > 1){
				info_new->refcount--;
			}
			else if(counting < 0){
				//printk("I am here 15_1\n");
				return -1;
			}
		}
		//printk("In mmap unmap\n");
		asm volatile("invlpg (%0);" :: "r"(itr) : "memory");
	}

	if(flag == 1){
		if((current_vm->vm_start > addr) && (current_vm->vm_end < addr+length)){
			//printk("I am here 13_4\n");
			if(stats->num_vm_area > 127){
				return -1;
			}
		}
	}

	if (flag == 2){
		current_vm = current->vm_area;
		while(current_vm != NULL){
			if((current_vm->vm_start < addr) && (current_vm->vm_end > addr+length)){
				if(stats->num_vm_area > 127){
					return -1;
				}
			}
			current_vm = current_vm->vm_next;
		}
	}


	if(flag == 1){
		//printk("I am here 13\n");
		//Case 1
		if((current_vm->vm_start == addr) && (current_vm->vm_end == addr+length)){
			//printk("I am here 13_1\n");
			if(buffer_vmap_area == NULL){
				current->vm_area = NULL;
				dealloc_vm_area(current_vm);
			}
			else{
				buffer_vmap_area->vm_next = current_vm->vm_next;
				dealloc_vm_area(current_vm);
			}
			return 0;
		}
		//Case 2
		else if((current_vm->vm_start == addr) && (current_vm->vm_end < addr+length)){
			//printk("I am here 13_2\n");
			current_vm->vm_start = current_vm->vm_start + length;
			return 0;
		}
		//Case 3
		else if((current_vm->vm_start > addr) && (current_vm->vm_end == addr+length)){
			//printk("I am here 13_4\n");
			current_vm->vm_end = current_vm->vm_end - length;
			return 0;
		}
		// Case 4
		else{
			//printk("I am here 13_5\n");
			struct vm_area* new_vmap_area;
			new_vmap_area = alloc_vm_area();
			new_vmap_area->vm_start = addr + length;
			new_vmap_area->vm_end = current_vm->vm_end;
			new_vmap_area->vm_next = current_vm->vm_next;
			new_vmap_area->access_flags = current_vm->access_flags;
			current_vm->vm_end = addr;
			current_vm->vm_next = new_vmap_area;
			return 0;
		}
	}
	else{
		//printk("I am here 14\n");
		//printk("%x : addr\n",addr);
		struct vm_area* current_vm;
		struct vm_area* previous_vm;
		previous_vm = NULL;
		current_vm = current->vm_area;
		while(current_vm != NULL){
			int flag_2 = 0;
			//printk("I am in an infinite loop 2\n");
			//printk("%x : start, %x : end, %x : addr, %x : addr+length\n",current_vm->vm_start,current_vm->vm_end,addr,addr+length);
			if((current_vm->vm_start >= addr) && (current_vm->vm_end <= addr+length)){
				if(previous_vm == NULL){
					//printk("I am here 14_1\n");
					current_vm = current_vm->vm_next;
					//dealloc_vm_area(current_vm);
					flag_2 = 1;
				}
				else{
					//printk("I am here 14_2\n");
					previous_vm->vm_next = current_vm->vm_next;
					//dealloc_vm_area(current_vm);
					flag_2 = 1;
				}
			}
			else if((current_vm->vm_start < addr+length) && (current_vm->vm_start >= addr)){
				//printk("I am here 14_3\n");
				current_vm->vm_start = addr+length;
			}
			else if((current_vm->vm_end > addr) && (current_vm->vm_end <= addr+length)){
				//printk("I am here 14_4\n");
				current_vm->vm_end = addr;
			}
			else if((current_vm->vm_start < addr) && (current_vm->vm_end > addr+length)){
				//printk("I am here 14_5\n");
				struct vm_area* new_vmap_area;
				new_vmap_area = alloc_vm_area();
				new_vmap_area->vm_start = addr + length;
				new_vmap_area->vm_end = current_vm->vm_end;
				new_vmap_area->vm_next = current_vm->vm_next;
				new_vmap_area->access_flags = current_vm->access_flags;
				current_vm->vm_next = new_vmap_area;
				current_vm->vm_end = addr;

			}
			//previous_vm = current_vm;
			//current_vm = current_vm->vm_next;
			if(flag_2){
				struct vm_area* replace;
				replace = current_vm;
				dealloc_vm_area(replace);
				current_vm = previous_vm->vm_next;
			}
			else{
				if(previous_vm){
					previous_vm = previous_vm->vm_next;
					current_vm = current_vm->vm_next;
				}
				else{
					previous_vm = current_vm;
					current_vm = current_vm->vm_next;
				}
			}
		}
		return 0;
	}	

	return isValid;
}
