# Discrete_Event_Simulation
Discrete_Event_Simulation


In this assignment you will construct a simulation of a video capture server and use this simulation to undertake performance analysis. The system can be modelled as tandem queue with two stages:

 

The system operation is as follows.
• Video enters the system in the form of fields. Each field is either a top field or a bottom field; the first field received will be a top field. A top field and the bottom field immediately following it make up a frame of video. Each frame of video is therefore comprised of two fields: a top field and a bottom field.
 
• Arriving fields are placed into a buffer prior to encoding. This buffer has the capacity to hold β fields. If either of the two fields of a frame is lost, the remaining field is useless. For this reason, the following actions are taken if an arriving field finds the buffer full:
– If the arriving field is a top field, it is discarded. The following bottom field will also be discarded whether or not upon its arrival there is space for it in the buffer.
– If the arriving field is a bottom field, it is discarded. The field that was last placed in the buffer, which will be a top field, is removed from the buffer and discarded.

• Each field requires processing by an encoder. The time required for this encoding depends on the complexity of the field, measured on fobs, and on the capacity of the encoder, expressed in fobs per second. Let the capacity of the encoder be Cenc (fobs/sec).

• Once encoded, the pairs of fields that make up video frames are processed together by the storage server as follows. When a top field is ready for processing by the storage server, it will remain in the queue—and the storage server will remain idle—until its corresponding bottom field also arrives. At that time, both fields will be removed from the queue and storage of the video frame will commence. The amount of time required to store the frame depends on its size in bytes and on the capacity of the storage server in bytes per second. If the complexities of the fields making up the frame are h1 and h2 then the frame’s size in bytes will be α(h1 + h2). Let the capacity of the storage server be Cstorage (bytes/sec). Furthermore, assume that there is always sufficient buffer space at the storage service facility to store an arriving field.

Suppose that we wish to study the capacity of encoder required to ensure that all frames are processed by the storage server. This means that no frames are lost because of lack of space in the encoder buffer and that the storage service facility is stable. Furthermore, we want to measure the utilization of the storage server.

(A)
Give the state variables, event graph, statistical counters, data structures required, and event types for the video storage system model. Also give the steps for the initialization routine, main program, timing routine, event handling routine for each event type, and the report generator. Your steps must include data collection for (i) the fraction of frames that are not stored because of lack of space in the encoder buffer and (ii) the utilization of the storage server. Assume that the first field, which will be a top field, arrives at time 0.

In providing your answer, you may use “determine inter_t” (i.e. a field inter-arrival time) if you need to determine a field inter-arrival time and “determine complexity” (i.e. a field complexity) if you need to determine the complexity of a field.


(B)
Write a simulation program (In C, C++, Java, Matlab,...) for this model. You are required to provide in-line documentation and proper indentation, clearly identifying the steps in the initialization routine, each event routine, and the report generator.
In constructing your simulation, you may assume the following about the operation of the system:
• The time between field arrivals is an exponential distribution with parameter τ= 1/120 seconds (or 1/250 seconds).
 • The complexity of a field follows an exponential distribution with parameter h= 200 fobs (or 450 fobs).
•α= 0.1, Cenc = 15800 and Cstorage = 1600.
Let f be the fraction of frames that are not stored because of lack of space in the encoder buffer and let u be the utilization of the storage server. Please run the simulation for eight hours and use your simulator to construct plots of f and u versus β for β= 20, 40, 60, 80, and 100.
