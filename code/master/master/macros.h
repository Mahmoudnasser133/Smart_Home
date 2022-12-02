/*
 * macros.h
 *
 * Created: 24-Nov-22 6:17:51 AM
 *  Author: nasser
 */ 


#ifndef MACROS_H_
#define MACROS_H_
#define STATUS_ADDRESS 0x00
#define admin_pass_ADDRESS1 0x01
#define admin_pass_ADDRESS2 0x02
#define admin_pass_ADDRESS3 0x03
#define admin_pass_ADDRESS4 0x04
#define guest_pass_ADDRESS1 0x05
#define guest_pass_ADDRESS2 0x06
#define guest_pass_ADDRESS3 0x07
#define guest_pass_ADDRESS4 0x08
#define block_address 0x09
#define block_address_data 0x10
#define max_tries 2

#define block_time_max 10 //in second
#define admin_time_max 15 //in second
#define guest_time_max 5 //in second

#define admin_time_flag 6 
#define admin_time_flag 7 
#define guest_time_flag 8 

#define block_led 2
#define guest_led 1
#define admin_led 0


#endif /* MACROS_H_ */