
#include<stdio.h>
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

int A_seq_counter;
int A_ack_counter; 
int B_seq_counter;
int B_ack_counter;

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



void A_output(message)
  struct msg message;
{

  // Sender's copy 
  struct pkt A_pckt_copy;

  // Message arriving from application layer
  A_seq_counter ++;
  // A_ack_counter ++;
  
  // Create the packet: Packet = seq + message + chcksum
  struct pkt packt_out;
  packt_out.seqnum = A_seq_counter;
  packt_out.acknum = A_ack_counter;
  strncpy(packt_out.payload, message.data, 20);

  // printf("\n\nPackt out Seq: %d Ack:%d Data:%s Checksum:%d",packt_out.seqnum,packt_out.acknum, packt_out.payload, packt_out.checksum);
  
  // Create a checksum value
  // printf("\n\nAddress of the packet %p",&packt_out);
  create_checksum(&packt_out);
  
  // Make a deep copy of the packet for retransmission
  A_pckt_copy = packt_out;
  printf("\n Transmitted packt seq %d",packt_out.seqnum);
  printf("\n Transmitted packt ack %d",packt_out.acknum);
  printf("\n Transmitted packt message %s",packt_out.payload);
  printf("\n Transmitted packt checksum %d",packt_out.checksum);

  // Keep packet in buffer - message in transit , previous ack not received yet
  // if (ack_not_received)
  // {
  //   // Create a buffer:
  //   struct pkt *buffer = (struct pkt *)malloc(1000);
  //   while(ack_not_received) 
  //   {

  //   }
  //   while (link_busy){
  //     printf("Busy");
  //   }

  // Then send the packet (in-order) waiting for ACK, keep a copy.
  
  tolayer3(0, packt_out) ;
  //starttimer(0, 20);
  // Wait for ACK message 
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
  // Received ACK ? timerinterrupt 

   int recvd_ACK_checksum  = calculate_checksum(&packet);
  
  if (packet.checksum == recvd_ACK_checksum)
  {
    printf("\n\n ACK received");
    A_seq_counter ++;
    // Send a new packet ACK to A 
  //   struct pkt ACK_packt;
  //   ACK_packt.acknum = B_ack_counter;
  //   ACK_packt.seqnum = B_seq_counter;
  //   strncpy(ACK_packt.payload,"",19);
  //   create_checksum(&ACK_packt);
  //   // create a packet
  //   struct pkt B_pckt_copy = ACK_packt;

  // // IF ALL GOOD , send to layer5 (up) and send ACK to tolayer3
  // // Send ACK tolayer3 - calls function A_input
  //   tolayer5(1,ACK_packt.payload);
  //   tolayer3(1,ACK_packt);

  // if timer expires: A_timerinterrupt is called 

    // retransmit packet with acknowledgement number we are waiting for


  // else:

    // receive the packet.
    // Validate the ACK checksum and see if it is the ACK for the message you just sent.

    // if good : Do nothing
        // else:
        //      retransmit
  }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
  // retransmit packet with acknowledgement number we are waiting for
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    // Initialize the sequence number
    A_seq_counter=0;
    A_ack_counter=0;
    // Initialize the acknowledgement number
    // 
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
struct pkt packet;
{
  // Packet sent from A contains :Packet = seq + message + chcksum

  // Check if in-order and corruption
  //   Validate the chcksum

  // See if the seq number is correct. If not correct, request 
  printf("\n Received packt seq %d",packet.seqnum);
  printf("\n Received packt ack %d",packet.acknum);
  printf("\n Received packt message %s",packet.payload);
  printf("\n Received packt checksum %d",packet.checksum);

  int recvd_packet_checksum  = calculate_checksum(&packet);
  
  if (packet.checksum == recvd_packet_checksum)
  {
    printf("\n\n Receiver received packet");
    B_ack_counter = packet.seqnum;
    // Send a new packet ACK to A 
    struct pkt ACK_packt;
    ACK_packt.acknum = B_ack_counter;
    ACK_packt.seqnum = B_seq_counter;
    strncpy(ACK_packt.payload,"",20);
    create_checksum(&ACK_packt);
    // create a packet
    struct pkt B_pckt_copy = ACK_packt;

  // IF ALL GOOD , send to layer5 (up) and send ACK to tolayer3
  // Send ACK tolayer3 - calls function A_input
    tolayer5(1,packet.payload);
    tolayer3(1,ACK_packt);
}
}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
 B_seq_counter = 0;
 B_ack_counter = 0;
}

// int main()
// { 
//   struct msg m1, m2 ,m3;

//   strncpy(m1.data,"Hello",19);
//   strncpy(m2.data,"Batman",19);
//   strncpy(m3.data,"Darryl",19);
//   A_output(m1);
//     A_output(m2);
//       A_output(m3);
// }