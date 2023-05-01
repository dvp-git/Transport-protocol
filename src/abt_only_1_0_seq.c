
#include<stdio.h>
#include<stdlib.h>
#include<string.h> // Added string library
#include "../include/simulator.h"


/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SIX ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */

// struct msg {
//   char data[20];
// };

// /* a packet is the data unit passed from layer 4 (students code) to layer */
// /* 3 (teachers code).  Note the pre-defined packet structure, which all   */
// /* students must follow. 
// struct pkt {
//    int seqnum;
//    int acknum;
//    int checksum;
//    char payload[20];
// };

// int A_prev_seq;
// int B_prev_ack;
int A_seq_counter;
int A_ack_counter; 
int B_seq_counter;
int B_ack_counter;
int A_ack_received;
struct msg A_buffer[1000];
int buffer_index;
struct pkt A_pckt_copy; 
struct pkt B_pckt_copy ;
// float A_old_expected_ack_time; 
// float A_new_expected_ack_time; 
void create_checksum(struct pkt *p)
{
  int checksum = 0;
  // printf("\n\n Inner pointer address %p", &p);
  // printf("\n\n Pointer value (should contain packet arguments address p %lx",p);

  // Add the seqnum and acknum
  checksum += p->seqnum;
  checksum += p->acknum;  

  // For payload extract each char, convert to int and add to checksum
  for( int i=0; i<20; i++)
  {
    //printf("\n\n %c",p->payload[i]);
    checksum += (int)p->payload[i];
    //printf("\n\n Checksum value : %d",checksum);
  }
  p->checksum = checksum;
  printf("\n\nCheck sum created : %d",p->checksum);
  return ;
}


int message_order_status(struct pkt *p, int seqnum)
{
  if (p->seqnum == seqnum)
    return 1;
  else 
    return 0;
}


int ACK_order_status(struct pkt *p, int seqnum)
{
  if (p->seqnum == seqnum)
    return 1;
  else  
    return 0;
}

int calculate_checksum(struct pkt *p)
{
  int checksum = 0;
  // printf("\n\n Inner pointer address %p", &p);
  // printf("\n\n Pointer value (should contain packet arguments address p %lx",p);

  // Add the seqnum and acknum
  checksum += p->seqnum;
  checksum += p->acknum;  

  // For payload extract each char, convert to int and add to checksum
  for( int i=0;i<20; i++)
  {
    //printf("\n\n %c",p->payload[i]);
    checksum += (int)p->payload[i];
    //printf("\n\n Checksum value : %d",checksum);
  }
  printf("\n\nCheck sum calculated : %d",checksum);
  printf("\n\nCheck sum actual : %d",p->checksum);
  return checksum;
}


struct msg pop(struct msg A_buffer[50])
{
  struct msg tmp;
  int i;
  if (A_buffer[0].data[0]!='\0')
  {
    strncpy(tmp.data, A_buffer[0].data, sizeof(A_buffer[0].data));
    for(i=0 ; A_buffer[i+1].data[0]!='\0'; i++)
      A_buffer[i] = A_buffer[i+1];
    strcpy(A_buffer[i].data,"\0");
  }
  return tmp;
}

int buffer_empty(struct msg A_buffer[50])
{
  printf("\nBuffer value first element %c",A_buffer[0].data[0]);
  if (A_buffer[0].data[0] == '\0')
    return 1;
  else
    return 0;
}

void A_output(message)
  struct msg message;
{
  printf("\n\n ACK value %d",A_ack_received);
  // BUffer not empty
  if (!A_ack_received)
  {
  // Store packets in A_buffer , previous ACK not received yet
  printf("\nMessage data %s",message.data);
  strncpy(A_buffer[buffer_index].data, message.data, 20);
  buffer_index ++;
  }
  else
  {
    struct msg message_1;
  // Start popping the elemnts
  if (A_seq_counter == 1 && buffer_empty(A_buffer))
    message_1 = message;
  else if (!(buffer_empty(A_buffer)))
  {
    strncpy(A_buffer[buffer_index].data, message.data, 20);
    message_1 = pop(A_buffer);
  }
  else
    message_1 = message;

  // Create the packet: Packet = seq + message + chcksum
    


  // Message arriving from application layer
  // A_seq_counter ++;
  // A_ack_counter += A_ack_counter + 32; // since size of a packet is 32
  struct pkt packt_out;
  packt_out.seqnum = A_seq_counter;
  packt_out.acknum = A_ack_counter;
  strncpy(packt_out.payload, message_1.data, 20);

   //printf("\n\nPackt out Seq: %d Ack:%d Data:%s Checksum:%d",packt_out.seqnum,packt_out.acknum, packt_out.payload, packt_out.checksum);
  
  // Create a checksum value
 // printf("\n\nAddress of the packet %p",&packt_out);
  create_checksum(&packt_out);
  
  // Make a deep copy of the packet for retransmission
  A_pckt_copy = packt_out;

  // Print out transmitted packet
 printf("\n Transmitting message packt seq %d",packt_out.seqnum);
 printf("\n Transmitting message packt ack %d",packt_out.acknum);
 printf("\n Transmitting message packt message %s",packt_out.payload);
 printf("\n Transmitting message packt checksum %d",packt_out.checksum);
  // printf("\n Length of packt %ld", sizeof(packt_out));
  // printf("\n Size of  int %ld", sizeof(int));



  // Then send the packet (in-order) waiting for ACK for 16 time units
  
  // starttimer(0,16);
  // float start_msg_trans = get_sim_time();

  starttimer(0, 15.0);
  A_ack_received = 0;
  // A_old_expected_ack_time = get_sim_time() + 15.0;
  printf("\nTimer A started");
  tolayer3(0, packt_out);
  printf("\nTransmission time %f",get_sim_time());
  }
  return;
}


void A_timerinterrupt()
{

 printf("\n----------Timer interrupt at %f------------",get_sim_time());
  // retransmit packet with acknowledgement number we are waiting for
  // while(A_ack_received==0)
  
  printf("\n\nRetransmission of packet");
  printf("\n\nnRetransmission Time : %f",get_sim_time());
  starttimer(0,15.0);
  // A_new_expected_ack_time = get_sim_time() + 15.0;
  tolayer3(0, A_pckt_copy);
    if (A_ack_received == 1)
  {
    return;
  }
}


/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
  // Received ACK ? timerinterrupt 

  printf("\n Receiving Ack packt time %f",get_sim_time());
  printf("\n Receiving ACk packt seq %d",packet.seqnum);
  printf("\n Receiving ACk packt ack %d",packet.acknum);
  printf("\n Receiving ACk packt message %s",packet.payload);
  printf("\n Receiving ACk packt checksum %d",packet.checksum);
 // Check order of ACK
  if (!A_ack_received)
  {
    // ACK delayed, ACK corrupt, ACK lost
  if (ACK_order_status(&packet, abs(A_seq_counter-1)))
  {
  int recvd_ACK_checksum  = calculate_checksum(&packet);
  if (packet.checksum == recvd_ACK_checksum)
    {
    A_ack_received = 1;

    printf("\nReceived Correct acknowledgement");
    printf("\nTimer A Stopped");
    printf("\n\n Acknowledgement Time : %f",get_sim_time());
    stoptimer(0);
    printf("\nACK received");
    printf("\n------------------------------");
    // A_prev_seq = A_seq_counter;
    A_seq_counter = abs(A_seq_counter - 1);
    A_ack_counter = abs(A_ack_counter - 1);
    }
  }
  return ;
  }
  else{
    printf("\n\n Duplicate ACK or ACK corrupt/lost : Awaiting latest ack");
    printf("\n\n Retranmission of ACK on the way due to corrupt/delayed ACK : %f",get_sim_time());
    // tolayer3(0, A_pckt_copy);
    // starttimer(0,15.0);
  return;
}
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    // Initialize the sequence number
    // A_prev_seq = 0;
    A_seq_counter=1;
    A_ack_counter=0;
    memset(A_buffer,'\0',sizeof(A_buffer));
    A_ack_received = 1;
    buffer_index=0;
    // Initialize the acknowledgement number
    // 
    return ;
}



/* Note that with simplex transfer from a-to-B, there is no B_output() */
/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
struct pkt packet;
{
  
  // Packet sent from A contains :Packet = seq + message + chcksum

  // Check if in-order and corruption
  //   Validate the chcksum
  printf("\n\n B Received packet Time : %f",get_sim_time());

  // See if the seq number is correct. If not correct, request 
  printf("\n Received message packt seq %d",packet.seqnum);
  printf("\n Received message packt ack %d",packet.acknum);
  printf("\n Received message packt message %s",packet.payload);
  printf("\n Received message packt checksum %d",packet.checksum);

  // Handle ordered delivery using the sequence numbers
  // CANNOT USE ACK RECEIVED SINCE IT IS A's VARIABLE
  // if (A_ack_received == 0)
  // {

  // int duplicate = duplicate(&packet, B_ack_counter);
  // if (!duplicate)
  // {
  if (message_order_status(&packet, B_seq_counter))
  {
    printf("\n\nReceived first time");
  int recvd_packet_checksum  = calculate_checksum(&packet);
  if (packet.checksum == recvd_packet_checksum)
    {
     printf("\n\n Receiver received Correct packet");
      B_seq_counter = abs(1-packet.seqnum);
      B_ack_counter = abs(1-packet.acknum);
      // Send a new packet ACK to A 
      struct pkt ACK_packt;
      ACK_packt.acknum = B_ack_counter;
      ACK_packt.seqnum = B_seq_counter;
      strncpy(ACK_packt.payload,"",20);
      create_checksum(&ACK_packt);
      // create a packet
      B_pckt_copy = ACK_packt;
    // IF ALL GOOD , send to layer5 (up) and send ACK to tolayer3
    // Send ACK tolayer3 - calls function A_input
      tolayer3(1,ACK_packt);
      tolayer5(1,packet.payload);
      printf("\n\n ACK Transmission time: %f",get_sim_time());
      printf("\n Transmitting ACK packt seq %d",ACK_packt.seqnum);
     printf("\n Transmitting ACK packt ack %d",ACK_packt.acknum);
     printf("\n Transmitting ACK packt message %s",ACK_packt.payload);
      printf("\n Transmitting ACK packt checksum %d",ACK_packt.checksum);
      // starttimer(1,15.0);
      // B_ack_counter += 33;
      // float b_ack_timer = get_sim_time();
      // Increment the ACK counter
    }
  return;
  }
   else{
   printf("\nINVALID checksum or duplicate packet");
   // Resending the ACK
   tolayer3(1,B_pckt_copy);
   }
   return;
  }

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
 B_seq_counter = 1;
 B_ack_counter = 0;
 B_pckt_copy.seqnum = 1;
 B_pckt_copy.acknum = 0;
 return ;
}

