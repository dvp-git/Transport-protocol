#include "../include/simulator.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
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

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
int A_ack_received;


int base;
int nextseqnum;
int Window_size;

int A_ack_counter; 
int B_seq_counter;
int B_ack_counter;


struct pkt send_buffer[1000]; 
// struct msg App_layer_buffer[1000];

int send_idx;
int store_idx;


// struct pkt A_pckt_copy; 
struct pkt B_pckt_copy ;


struct pkt create_pkt(struct msg *M, int seqnum , int acknum)
{
  struct pkt packt_out;
  packt_out.seqnum = seqnum;
  packt_out.acknum = acknum;
  strncpy(packt_out.payload, M->data, 20);
  return packt_out;
}




void create_checksum(struct pkt *p)
{
  int checksum = 0;

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
  //if (p->seqnum == (seqnum - 1))
  if (p->seqnum == (seqnum - 1))
    return 1;
  else 
    return 0;
}



int ACK_order_status(struct pkt *p, int base)
{
  if (p->acknum >= base)
    return 1;
  else  
    return 0;
}

int calculate_checksum(struct pkt *p)
{
  int checksum = 0;

  // Add the seqnum and acknum
  checksum += p->seqnum;
  checksum += p->acknum;  

  // For payload extract each char, convert to int and add to checksum
  for( int i=0;i<20; i++)
  {
    checksum += (int)p->payload[i];
  }
  printf("\n\nCheck sum calculated : %d",checksum);
  printf("\n\nCheck sum actual : %d",p->checksum);
  return checksum;
}


// Method to pop packet from buffer  
struct msg pop(struct msg send_buffer[50])
{
  struct msg tmp;
  int i;
  if (send_buffer[0].data[0]!='\0')
  {
    strncpy(tmp.data, send_buffer[0].data, sizeof(send_buffer[0].data));
    for(i=0 ; send_buffer[i+1].data[0]!='\0'; i++)
      send_buffer[i] = send_buffer[i+1];
    strcpy(send_buffer[i].data,"\0");
  }
  return tmp;
}


// Return true if buffer is empty
int buffer_empty(struct pkt p, int seqnum)
{
  if (p.payload[0] == '\0')
  {
    printf("\nBuffer empty Evidence: %c", p.payload[0]);
        return 1;
  }
  else
  {
   printf("\n BUF NOT EMPTY: Buffer value of packet %d  is  %c",seqnum, p.payload[0]);
    return 0;
  }
}


/* called from layer 5, passed the data to be sent to other side */
void A_output(message)
  struct msg message;
{
  printf("\n\nBase : %d, NextSeqNum: %d",base,nextseqnum);
  struct pkt packetout;
  printf("\n\nMessage arriving from application layers %s",message.data);
  // Two buffers available : App_layer_buffer and Send_buffer
  // - Sample : Window size is 10.
  // - 10 messages trasnmitted first. [ Move it to the window buffer, and transmit.]
  //   - base 0 to 10 increase iff [ nextseqnum < base + Window] : SEND THE PACKETS / transmit
  //   - if nextseqnum == base + Window : STOP SENDING THE PACKETS, STORE THEM in the buffer indefinitely using store index, i.e Update store index to the latest next sequence number and store it in the buffer
  //   - if ACK is received. 

// Window =  10
  if (nextseqnum < (base + Window_size))
  {
    if (buffer_empty(send_buffer[nextseqnum],nextseqnum))
    {
      // If buffer value is empty create packet and put values.
      packetout = create_pkt(&message, nextseqnum , A_ack_counter);
      create_checksum(&packetout);
      send_buffer[nextseqnum] = packetout;
    }
    // else Store has already put the value in this. Send the buffer[nextseq]
      // Transmitting messages 
      printf("\n Transmitting message packt seq %d",send_buffer[nextseqnum].seqnum);
      printf("\n Transmitting message packt ack %d",send_buffer[nextseqnum].acknum);
      printf("\n Transmitting message packt message %s",send_buffer[nextseqnum].payload);
      printf("\n Transmitting message packt checksum %d",send_buffer[nextseqnum].checksum);

      // Send packt to B in until window is full
      tolayer3(0, send_buffer[nextseqnum]);

        // Window Full, start timer
        if (base == nextseqnum)
        {
          // win_L += Window_size;
          // win_U += Window_size;
          stoptimer(0.0);
          printf("\nTimer A started");
          starttimer(0,20.0);
          printf("\nTransmission time of oldest unack'd packt %f",get_sim_time());
        }
      // Increment sequence number
      nextseqnum  = (nextseqnum + 1) ;
      printf("\nNextSeqNum %d",nextseqnum);
       // }
    }
    else
    {
      if (store_idx < (nextseqnum))
      {
        store_idx = (nextseqnum);
      }
      printf("\nStore Idx: %d , NextSeqNum : %d", store_idx, nextseqnum);
      packetout = create_pkt(&message, store_idx , A_ack_counter);
      create_checksum(&packetout);
      send_buffer[store_idx] = packetout;
      printf("\n\nStoring the packet %d with data %s in the buffer: Waiting for ACKs",store_idx,send_buffer[store_idx].payload);
      store_idx ++;
    }
}


void A_timerinterrupt()
{
  printf("\n----------Timer interrupt at %f------------",get_sim_time());
  // retransmit packet with acknowledgement number we are waiting for
  // while(A_ack_received==0)
  printf("\nBase : %d , Next Seq: %d",base,nextseqnum);
  // printf("\n\nRetransmission of packets");
  starttimer(0,20.0);
  // Made changes tonextseqnum 
    for (int i=base; i < nextseqnum ; i++)
    {
      printf("\n\nnRetransmission Time : %f packet no: %d",get_sim_time(),i); 
      tolayer3(0, send_buffer[i]);
    }
  // else
  // { return;
  // }
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

  printf("\nExpecting Ack seqnum %d",base+1);
 // Check order of ACK
  if (ACK_order_status(&packet, base+1))
  {
  int recvd_ACK_checksum  = calculate_checksum(&packet);
  if (packet.checksum == recvd_ACK_checksum)
    {
    A_ack_received = 1;
    base = packet.acknum ;
    //Removed window size : Window_size += 1;
    printf("\nReceived Correct acknowledgement");
    printf("\n\n Acknowledgement Time : %f",get_sim_time());
    printf("\nACK received");
    printf("\n------------------------------");
    if (base == nextseqnum)
    {
      printf("\nbase == nextseqnum");
      stoptimer(0);
        if (!(buffer_empty(send_buffer[nextseqnum], nextseqnum)))
        {
      // Made changes tonextseqnum 
          for (int i = nextseqnum; i < base + Window_size ; i++)
          {
          printf("\n\n Transmitting next : %f packet no: %d seqnum: %d ",get_sim_time(),i,send_buffer[i].seqnum); 
          tolayer3(0, send_buffer[i]);
          printf("\n\n ----DEBUG--- %d",send_buffer[i].seqnum);
          nextseqnum++;
          }
      }
    }
    else{
      printf("\nTimer A re-started");
      stoptimer(0);
      starttimer(0,20.0);
    /* ADDING NEW CODE TO SEND PACKETS AS SOON AS ACK RECEIVED HERE - BIG BUG : DONT send packets again after acknowledgement. Send only next window packets*/
      printf("\nAck received for previous packet no: %d",base-1); 
     /*for (int i = base; i < nextseqnum ; i++)
      {
      if (!(buffer_empty(send_buffer[i], nextseqnum)))
      {
      printf("\n\n Transmitting next : %f packet no: %d seqnum: %d ",get_sim_time(),i,send_buffer[nextseqnum].seqnum); 
      tolayer3(0, send_buffer[i]);
      //printf("\n\n ----DEBUG--- %d",&send_buffer[i].seqnum);
      printf("\n\n ----DEBUG--- %d",send_buffer[i].seqnum);
      nextseqnum++;
      }
      }*/
    }
  }
  return ;
  }
  else{
    printf("\n\n ACK received is corrupt/delayed : Awaiting new packet");
    printf("\n\n Retranmission on the way due to corrupt/delayed ACK : %f",get_sim_time());
  return;
}
}

/* called when A's timer goes off */



/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    // Initialize the sequence number
    // A_prev_seq = 0;

    //A_seq_counter = 1;
    A_ack_counter = 0;

    base = 0;
    nextseqnum = 0;
    // Set all the app layer buffer '\0'
    // memset(App_layer_buffer,'\0',sizeof(App_layer_buffer));
    A_ack_received = 1;


    // Get window size from running program and set all the window buffer to '\0'
    Window_size = getwinsize();
    // win_L = 0;
    // win_U = Window_size - 1;

    send_idx = 0;
    store_idx = Window_size;

    // send_buffer = malloc(Window_size * sizeof(struct msg));
    // memset(send_buffer,'\0',Window_size * sizeof(*send_buffer));
   // send_buffer = malloc(1000 * sizeof(struct pkt));
    memset(send_buffer,'\0',sizeof(*send_buffer)*1000);
    printf("First value send_buffer : %s",send_buffer[0].payload);
    printf("\nSize of send_buffer %ld",sizeof(*send_buffer)*1000);
    // printf("\nSize of packet", sizeof(struct pkt));

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

  printf("\nExpecting packet seqnum %d",B_seq_counter-1);
  // Handle ordered delivery using the sequence numbers
  // CANNOT USE ACK RECEIVED SINCE IT IS A's VARIABLE
  if (message_order_status(&packet, B_ack_counter))
  {
  printf("\n\nReceived first time");
  int recvd_packet_checksum  = calculate_checksum(&packet);
  if (packet.checksum == recvd_packet_checksum)
    {
     printf("\n\n Receiver received Correct packet");
      // B_prev_ack = B_ack_counter;
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
      tolayer5(1,packet.payload);
      printf("\n\n ACK Transmission time: %f",get_sim_time());
      tolayer3(1,ACK_packt);


      printf("\n Transmitting ACK packt seq %d",ACK_packt.seqnum);
      printf("\n Transmitting ACK packt ack %d",ACK_packt.acknum);
      printf("\n Transmitting ACK packt message %s",ACK_packt.payload);
      printf("\n Transmitting ACK packt checksum %d",ACK_packt.checksum);
      // starttimer(1,15.0);
      B_seq_counter =  B_seq_counter + 1 ;
      B_ack_counter =  B_ack_counter + 1 ;
      // float b_ack_timer = get_sim_time();
      // Increment the ACK counter
    }
  return;
  }
  else{
   printf("\nINVALID checksum or duplicate packet");
   tolayer3(1,B_pckt_copy);
   return;
  }
}
/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
//  B_prev_ack = 0;
 B_seq_counter = 1;
 B_ack_counter = 1;
 B_pckt_copy.seqnum = 0;
 B_pckt_copy.acknum = 0;
 return ;
}

