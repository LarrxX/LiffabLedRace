# Serial Protocol  (OLRBoard←→Host)




- - -

**Revisions:**
-

- - -
-
In the present doc the terms Board and Host indicate:
   - ***Board***: OLR Board - The microcontroller managing the led strip (Arduino)
   - ***Host***: The Host running the OpenLedRace COnfiguration software (PC, RPI, etc)

The Board is currently connected to the Host via Serial interface (USB)

## Implementation characteristic

    1. Local Communication between Board and Host use plain ASCII (not binary).

    2. Messages are kept very short (lightweight protocol for “speed” and low resources.)


## Message formats
   - Messages are composed by 2 parts: **Command**, **Parameters**.
   - Messages with multiple parameters, the char **'.'** is used as **"Parameters Separator"**
   - Messages sending back command confirmation uses "**<i>command</i>OK**" and "**<i>command</i>NOK**"
     - Example: **CNOK** is the 'error' answer sent for a C command
   - Telemetry Messages (see below) have fixed lenght parameters, and no
     message terminator.
   - Any other message ends with the "EOC" (End Of Command) char:
     - [LF] - Line Feed = ASCII 10/0x0A = new line = ‘\n’
   - The following command, for example, is used by a OLR Device (Arduino)
     to send the “Setup OK - Configuration Complete” message:
     -  `R0[LF] `

* * *

## Commands list
Command | Description                     | Notes
--------|---------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
\#      | Protocol Handshaking            | Host-Board handshake on startup
@       | Reset                           | Host send a Reset to the Board
$       | Get UID                         | Get Board Unique Id
%       | Get Version                     | Get Board Software version
:       | Set Unique ID                   | Set Board Unique ID
!       | Send log/error msg              | Send a log/error message to peer
**C**   | **C**onfiguration Race          | Set basic race configuration
**Q**   | Query board cfg                 | Host request the current situation of the Config Parameters Set
**R**   | Race phase                      | Command used to notify current Race phase 
**T**   | Track configuration             | Command used to configure a new track setup.
**A**   | Ramp configuration              | Command used to configure a ramp in the track.
**D**   | Load Track and ramp defult      | Command used to set default parameter for ramp and track
**p**   | Car current position in OLR     | Current position of the car in the OLR
**w**   | Car Win the Race                 | A car just win the current race


* * *

## Commands Description


In the following sections the column "**Initiate**" contains the id of the board sending the message.
   - ***B*** - ***Board***: OLR Board (Arduino Nano)
   - ***H*** - ***Host***: Host where the OpenLedRace Manger program is running (RPI, PC, ...).

Same rule applies to the "***From***" column in "***Response***"

Some commands may be originated by both peers (ex: Handshake command)

The string **_[LF]_** indicates the EOC (End of Command) char = "line feed / new line", (ASCII 10/0A)


- - -

### [#] - Protocol Handshaking [Implmented]
|initiate| Syntax | Description
|------|--------|------------
|B, H|**#**_[LF]_ | **Protocol Handshaking**
||   | Sent to initialize a connection (Board and Host) |
|**Response**| **From** | **Notes**
|**#**_[LF]_ |H, B    | The connection opens succesfully when a “**#**” is received 'back' from the peer
||| **Please Note:**
||| Board and Host send back only one ACK (send back a '#' just once)

- - -

### [@] - Reset [To be implemented]
|initiate| Syntax | Description
|------|--------|------------
|H |**@**_[LF]_|**OLR Board Reset request**
| |   | Sent from Host to Reset the OLR Board to the initial state (before handshake)
|**Response**| **From** | **Notes**
| |     | No response expected from Board


- - -

### [$] - Get Board UID [Implemented]


| initiate | Syntax      | Description                        |
|----------|-------------|------------------------------------|
| H        | **$**_[LF]_ | **OLR Board UID request**         |
|          |             | Sent from Host to get Board's UID |

|**Response**| **From** | **Notes**
|------|--------|------------
|**$**Id[LF] | B  | Send the UID strings

#### UID String format
-- to be defined



#### Examples
|Origin|Command||
|---|----|----|
|H|**$[LF]**| Host send a **get info** request
|B|**?3179c3ec6e28a[LF]**|The message from the Board contains Id="3179c3ec6e28a"


|Origin|Command||
|---|----|----|
|H|**?[LF]**| Host send a **get info** request
|B|**?[LF]**|The message from the Board indicates ID not set.


- - -

### [%] - Get Software Version [Implemented]

Used to check software  compatibility between Board and Host program's versions

| initiate | Syntax      | Description                        |
|----------|-------------|------------------------------------|
| H        | **%** [LF]  | **OLR Board software version request**         |


|**Response**| **From** | **Notes**
|------|--------|------------
|**%**Ver[LF] | B  | String representing the Software Version


#### Software Version String format
-- to be defined


#### Examples
|Origin|Command||
|---|----|----|
|H|**%[LF]**| Host send a **get info** request
|B|**%1.0[LF]**|The message from the Board indicates Version="1.0.1"

- - -


### [:] - Set board Unique Id [Implemented]


The software running on the Board contains a routine to write an ID to EEPROM.<br>
The first time a Board is connected to a OLRNetwork the Unique Id may be empty (not every Board comes with the ID preloaded in EEPROM).<br>
The Host calculates an ID and send this **Set Id** command to the Board that will store
it in EEPROM.<br>
From now on this is the ID the board will send back when receiving  a **Get Info '$'** command.


|initiate| Syntax | Description
|------|--------|------------
|H |**:**id[LF]|**OLR Board Set UniqueId request**
||   | Sent from Host to Set Board's Unique Id

|Parameters  |   |   |
|----------------|---|---|
| |id: | String representing the Unique Id. The string contains 16 characters max.


|Response| |
|---|---
|**OK**[LF]  | Board sends "OK" string (ACK)
|**NOK**[LF] | Board indicates that something went wrong  


- - -

### [!] - Send log/error message [Work in progress]

The software running on the Board use this command to send messages to be written into the Host logfile.<br>
The Host will log the message and decide what to do with the relay race according to the "Severity" parameter (nothing, stop it, etc.)


|initiate| Syntax | Description
|------|--------|------------
|B |**!**Severity,Message[LF]|**OLR Board Sends an error message to Host**

|Parameters  |   |   |
|----------------|---|---|
| Severity:[0-3] |  single char|
|   |  1| **Log only** - Board want to log a message into the Host Message LogFile,<br> Sent usually in development/debug phase to trace the dialog between Board and Host |
|   |  2| **Warning** - Board send back a "warning" message<br>Sent by board on 'not blocking' errors like, for example, unknown commands or parameters  |
|   |  3| **Blocking Error** - The boards have a Severe error condition and cannot proceed.<br> The Host will log the message into the Host Message LogFile and decide what to do (if the Host is running a RelayRace it will Stop the Race)<br>
|   |   |   |
| Message:String |ASCII| The ASCII String containing the message.

|**Response**| **From** | **Notes**
|------|--------|------------
|      | H  | No answer sent from Host

#### Examples
|Origin|Command||
|---|----|----|
|B|**\!1,invalid Car=[3] in [t] command**| Board send a warning message about a previously received command

- - -

### [**C**] - Set basic race configuration [Implemented]


|initiate| Syntax | Description
|------|--------|------------
|H |**C**start.nlap.repeat.finish[LF] |**Host Set basic race configuration**



#### Parameters

| Parameter |Format| Description
|--------|---|---------
| **start** | [0-1] |**Start Line** of the race is in this Board (Y/N) (0=No, 1=Yes)
| **nlap** | [1-9][0-9]? |Number of consecutive laps in each **section** of the Relay Race
| | | max 2 chars (range 1-99)<br>Number of consecutive laps the cars will “run” before race finish **or** car get trough the OutTunnel
| **repeat** |[1-9][0-9]?| Number of times to **repeat the configured section** of ‘nlap’ laps.
| | | max 2 chars (range 1-99)<br>How many times the section of ‘L’ laps will be repeated
| **finish** |[0-1] |**Finish Line** of the race is in this Board (Y/N) (0=No, 1=Yes)



|Response| |
|---|---
|**OK**[LF]  | Board sends "OK" string (ACK)
|**NOK**[LF] |Board indicates that something went wrong    


#### Examples
|Origin|Command||
|---|----|----|
|H|**C0,5,2,1**|**start=0**: The Race starts in another OLR – The Board will be waiting for messages like “Race Started”, “Car 1 Leaving”, Car 1 Leaved”, etc...    
|||**laps=5**: Each car will need to complete 5 laps before it can cross the Finish Line **or** get to the next OLR through the OutTunne.<br>(see 'repeat' param)
|||**repeat=2**: Each car will need to repoeat 2 times the section of ‘nlap’ laps.<br>This means we’ll expect each car will be sent back here (through the InTunnel) after we previously sent it out to another Racetrack.
|||**finish=1**: The Race ends here.This OLR will manage the Finsh Line Procedure.
|B|**OK[LF]**| This is the Response from the Board to the previous example(ACK)
|||The message from the Board indicates that the value for Position,laps, repeat and finish line has been set correctly as requested by the host.
|   |   |   |
|H|**C1,2,3,0[LF]**| Position in Race is “1”  The Race starts here
|||The Board will be managing the Start Race phase (Semaphore, etc.)   |
|||Each car will need to complete 2 loops here before can get to the next Racetrack (through the OutTunne) .
|||Each car will need to repoeat 3 times the section of ‘nlap’ laps.
|||The Race ends in another OLR.
|B|**NOK[LF]**| This is a Response from the Board (ACK)
|||The [NOK] value from the Board indicates that something went wrong (the board received some  invalid paramenter value).     |


- - -

### [T] - **T**rack configuration [ Implemented ]

This configuration is stored in non-volatile memory.

|initiate| Syntax | Description
|------|--------|------------
|H |**T**box.tbd[LF] |**Host Set basic race configuration**


#### Parameters

| Parameter |Format| Description
|--------|---|---------
| **box** | [0-MAXLED] | Number of the led where the box starts. Set 0 to remove box.
| **tbd** | [TBD] | Not used yet, set to 0.


|Response| |
|---|---
|**TOK**[LF]  | Board sends "OK" string (ACK)
|**TNOK**[LF] |Board indicates that something went wrong   

|Origin|Command||
|---|----|----|
|H|**T260,0**|: Set the box line in led number 260.

- - -

### [A] - r**A**ramp configuration [ Implemented ]

This configuration is stored in non-volatile memory.

|initiate| Syntax | Description
|------|--------|------------
|H |**A**center.high[LF] |**Host Set basic race configuration**


#### Parameters

| Parameter |Format| Description
|--------|---|---------
| **center** | [0-MAXLED] | Number of the led where ramp is centered. Set 0 to remove box.
| **height** | [ 0 - 1023] | Ramp elevation


|Response| |
|---|---
|**AOK**[LF]  | Board sends "OK" string (ACK)
|**ANOK**[LF] |Board indicates that something went wrong   

|Origin|Command||
|---|----|----|
|H|**A150,12**|: Set the ramp centered in led 150.

- - -

### [D] - **D**efault configuration [ Implemented ]

Set default configuration to track and ramp settings. 
|initiate| Syntax | Description
|------|--------|------------
|H |**D**[LF] |**Host Set basic race configuration**



- - -


### [**p**] - Current Car **p**osition in Race [Implemented]
|initiate| Syntax | Description
|------|--------|------------
|B|**p**NumTrackNlapRpos[LF]|**Position for each car in the race**
|||Sent during race for each car currently in this Board.

|Parameters    |   |   |
|--------------|---|---|
| Num [1-9]    |   | One char representing Car Number.
| Track [A-Z]  |   | One char representing the Track where the car is.
|              | M | Main Track |
|              | B | Box Track  |
|              | U | Not Track  |
|              | . | ....       |
| Nlap [1-99]  |   | Number of the Current Lap.
| Rpos [00-99] |   | Relative position in a track in percent.

|Response| From | |
|---|--- |---
| | H | No response from host


#### Examples
|Origin|Command||
|---|----|----|
|B|**p**1B1.95**p**2M5.45[LF]| Two cars are currentry "running" in the Board. Car "1" is in Track "B" in Lap number "1" Relative Lap Position 95%. Car "2" is in Track "M" in Lap number "5" Relative Lap Position 45%


- - -

### [w] - Car Win te Race [Implemented]

|initiate| Syntax | Description
|------|--------|------------
|B|**w**Num[LF]|**Car 'Num' just win the race**
|||Sent by the circuit managing the "Finish Line" when a car cross it.

|Parameters  |   |   |
|----------------|---|---|
| Num [1-9] | |One char representing Car Number

|Response| From | |
|---|--- |---
| | H | No response from host


#### Examples
|Origin|Command||
|---|----|----|
|B|**w**1| Car "1" win the race

30_Network_Protocol_Serial.md
Mostrando 30_Network_Protocol_Serial.md.