# Serial Protocol  (OLRBoard←→Host)


In the present doc the terms Board and Host indicate:
   - ***Board***: OLR Board - The microcontroller managing the led strip (Arduino)
   - ***Host***: The Host running the OpenLedRace COnfiguration software (PC, RPI, etc)

The Board is currently connected to the Host via Serial interface (USB)

## Implementation characteristic

    1. Local Communication between Board and Host use plain ASCII (not binary).

    2. Messages are kept very short (lightweight protocol for “speed” and low resources.)


## Message formats
   - Messages are composed by 2 parts: **Command**, **Parameters**.
   - Messages with multiple parameters, the char **','** is used as **"Parameters Separator"**
   - Messages sending back command confirmation uses "**<i>command</i>OK**" and "**<i>command</i>NOK**"
     - Example: **CNOK** is the 'error' answer sent for a **C** command
   -Messages end with the "EOC" (End Of Command) char:
     - [EOC] - Line Feed = ASCII 10/0x0A = new line = ‘\n’

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
**C**   | **C**onfiguration Race          | Set basic race configuration (Number of **laps** for a single race)
**R**   | Race phase                      | Command used to Change/Notify current Race phase
**T**   | Track length configuration      | Command used to configure the Total Number of LEDs in the track.
**B**   | Box length configuration        | Command used to configure the number of LEDS at the end of the LED Strip reserved for Boxes (Pitlane).
**A**   | Ramp configuration              | Command used to configure the Ramp in the track.
**D**   | Load Track and ramp defult      | Command used to Reset to Default parameters (Race, Ramp, Pitlane)
**Q**   | Query board cfg                 | Host request the current situation of the Config Parameters Set
**p**   | Car current position in OLR     | Current position of the car in the OLR
**w**   | Car Win the Race                 | A car just win the current race


* * *

## Commands Description


In the following sections the column "**Initiate**" contains the id of the board sending the message.
   - ***B*** - ***Board***: OLR Board (Arduino Nano)
   - ***H*** - ***Host***: Host where the OpenLedRace Manger program is running (Computer).

Same rule applies to the "***From***" column in "***Response***"

Some commands may be originated by both peers (ex: Handshake command)

The string **_[EOC]_** indicates the EOC (End of Command) char, currently = "line feed", (ASCII 0A)


- - -

### [#] - Protocol Handshaking
|initiate| Syntax | Description
|------|--------|------------
|B, H|**#**_[EOC]_ | **Protocol Handshaking**
||   | Sent to initialize a connection (Board and Host) |
|**Response**| **From** | **Notes**
|**#**_[EOC]_ |H, B    | The connection opens succesfully when a “**#**” is received 'back' from the peer


- - -

### [@] - Reset [To be implemented]
|initiate| Syntax | Description
|------|--------|------------
|H |**@**_[EOC]_|**OLR Board Reset request**
| |   | Sent from Host to Reset the OLR Board to the initial state (before handshake)
|**Response**| **From** | **Notes**
| |     | No response expected from Board


- - -

### [$] - Get Board UID


| initiate | Syntax      | Description                        |
|----------|-------------|------------------------------------|
| H        | **$**_[EOC]_ | **OLR Board UID request**         |
|          |             | Sent from Host to get Board's UID |

|**Response**| **From** | **Notes**
|------|--------|------------
|**$**Id[EOC] | B  | Send the UID strings

#### UID String format
Unique Board Id (UID) string format:<br>
**^[\x33-\x7E]{16}$**

- Lenght: 	16 chars
- Valid Chars: Ascii 7-bit Printable Chars excluding ‘space’=ASCII 32 (this means ASCII chars between 33 (0x21) and 126 (0x7E) inclusive





#### Examples
|Origin|Command||
|---|----|----|
|H|**$[EOC]**| Host send a **get info** request
|B|**?3179c3ec6e28a[EOC]**|Board answer: Id="3179c3ec6e28a"


|Origin|Command||
|---|----|----|
|H|**?[EOC]**| Host send a **get info** request
|B|**??????????[EOC]**|The Board send back an invalid UID (if you are looking at it in a Serial Console, you usually see a bunch of question marks or other chars / non-printable ASCII).<br> This usually happens when the UID is not set yet, so  the Board send back the contents of the area of the EEPROM where the UID is supposed to be stored.


- - -

### [%] - Get Software Version

Used to check software  compatibility between Board and Host program's versions

| initiate | Syntax      | Description                        |
|----------|-------------|------------------------------------|
| H        | **%** [EOC]  | **OLR Board software version request**         |


|**Response**| **From** | **Notes**
|------|--------|------------
|**%**Ver[EOC] | B  | String representing the Software Version


#### Software Version String format
[0-9]+\.[0-9]+\.[0-9a-zA-Z]+

Two dot-separated decimal numbers plus a third part composed by numbers and/or letters.


#### Examples
|Origin|Command||
|---|----|----|
|H|**%[EOC]**| Host send a **get info** request
|B|**%0.9.d[EOC]**|The message from the Board indicates Version="0.9.d"

#### Guidelines to Assign a version number to the Arduino Software:


The three numbers represents the “Major.Minor.Patch” version.

    • Major version zero (0.y.z) is for initial development. Anything MAY change at any time.
    • Version 1.0.0 defines first ‘Stable’ version
    • Increment:
        ◦ MAJOR version when you make incompatible changes
        ◦ MINOR version when you add functionality in a backwards compatible manner
        ◦ PATCH version when you make backwards compatible bug fixes.





- - -


### [:] - Set board Unique Id


|initiate| Syntax | Description
|------|--------|------------
|H |**:**id[EOC]|**OLR Board Set UniqueId request**
||   | Sent from Host to Set Board's Unique Id

|Parameters  |   |   |
|----------------|---|---|
| |id: | String representing the Unique Id. The string contains 16 characters max.


|Response| |
|---|---
|**OK**[EOC]  | Board sends "OK" string (ACK)
|**NOK**[EOC] | Board indicates that something went wrong  


- - -

### [!] - Send log/error message

The software running on the Board use this command to send messages to be written into the Host logfile.<br>
The Host will log the message and decide what to do with the relay race according to the "Severity" parameter (nothing, stop it, etc.)


|initiate| Syntax | Description
|------|--------|------------
|B |**!**Severity,Message[EOC]|**OLR Board Sends an error message to Host**

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

### [**C**] - Set basic race configuration

This configuration is stored in non-volatile memory.


|initiate| Syntax | Description
|------|--------|------------
|H |**C**start.nlap.repeat.finish[EOC] |**Host Set basic race configuration**



#### Parameters

| Parameter |Format| Description
|--------|---|---------
| **start** | [0-1] | OLRNetwork Only **Always 1 for standalone OLR**
| **nlap** | [1-9][0-9]? |Number of laps of a Race
| | | max 2 chars (range 1-99)
| **repeat** |[1-9][0-9]?| OLRNetwork Only **Always 1 for standalone OLR**
| | |
| **finish** |[0-1] | OLRNetwork Only **Always 1 for standalone OLR**



|Response| |
|---|---
|**OK**[EOC]  | Board sends "OK" string (ACK)
|**NOK**[EOC] |Board indicates that something went wrong    


#### Examples
|Origin|Command||
|---|----|----|
|H|**C1,3,1,1**|**laps=3**: Each car will need to complete 3 laps before it can cross the Finish Line
|B|**OK[EOC]**| This is the Response from the Board to the previous example(ACK)
|||The message from the Board indicates that the value for Position,Laps, Repeat and Finish line has been set correctly as requested by the host.


- - -

### [T] - **T**rack configuration - Total LEDs Number

This configuration is stored in non-volatile memory.

|initiate| Syntax | Description
|------|--------|------------
|H |**T**nled[EOC] |**Host Set Racetrack Length Configuration**


#### Parameters

| Parameter |Format| Description
|--------|---|---------
| **nled** | Total number of LEDs in the Track | Ex: 300 for a single 5mt - 60 LED/mt LED Strip <br>**Please Note:<br>**After changing the LEDs number ** you need to reboot the board **

|Response| |
|---|---
|**TOK**[EOC]  | Board sends "OK" string (ACK)
|**TNOK**[EOC] |Board indicates that something went wrong   

|Origin|Command||
|---|----|----|
|H|**T600**| Total Length is 600 (2 x 300 LED Strip connected).


- - -

### [B] - **T**rack configuration - Pitlane Lenght (Boxes)

This configuration is stored in non-volatile memory.

|initiate| Syntax | Description
|------|--------|------------
|H |**B**nled[EOC] |**Host Set Pitlane (boxex) Length Configuration**


#### Parameters

| Parameter |Format| Description
|--------|---|---------
| **nled** | Total number of LEDs, at the end of the Racetrack, reserved for the Pitlane | Ex: 120

|Response| |
|---|---
|**BOK**[EOC]  | Board sends "OK" string (ACK)
|**BNOK**[EOC] |Board indicates that something went wrong   

|Origin|Command||
|---|----|----|
|H|**B120**| Total Length for Pitlane is 120




- - -

### [A] - r**A**ramp configuration

This configuration is stored in non-volatile memory.

|initiate| Syntax | Description
|------|--------|------------
|H |**A**start,center,end,high[EOC] |**Host Set basic Ramp configuration**


#### Parameters

| Parameter |Format| Description
|--------|---|---------
| **start** |  | LED number where the ramp Starts
| **center** |  | LED Number where ramp is centered.
| **end** |  | LED number where the ramp ends
| **height** | [ 0 - 1023] | Ramp elevation


|Response| |
|---|---
|**AOK**[EOC]  | Board sends "OK" string (ACK)
|**ANOK**[EOC] |Board indicates that something went wrong   

|Origin|Command||
|---|----|----|
|H|**A140,150,160,12**|: Set the ramp centered in led 150, starts 10 LED before it and ends 10 LEDs after it, Elevation 12.

- - -

### [D] - **D**efault configuration [ Implemented ]

Reset to default configuration parametrs (Track lenght, Ramp, Boxes and Race Laps).

|initiate| Syntax | Description
|------|--------|------------
|H |**D**[EOC] |**Host Request Board to Reset configuration parameters to Default**


- - -


### [Q] - Query current parameters set


| initiate | Syntax      | Description                        |
|----------|-------------|------------------------------------|
| H        | **Q**_[EOC]_ | **Get Current Parameters request**         |
|          |             | Sent from Host to get Board's Cfg |


#### Returned Parameters format


|**Response**| **From** | **Notes**
|------|--------|------------
|Board issue 3 answers:<br>**TRACK:**a,b,c,d,e[EOC]<br> **RAMP:**a,b,c,d[EOC]<br> **RACE:**a,b,c,d[EOC]<br>| B  | Send the Parameters Set
|   |    |   |
| **TRACK** params  |   |   |
|                   | **nled_total**  | Total number of LEDs in the Racetrack (**configurable with "T" command**)  |
|                   | nled_main  | Internal parameter (when Pitlane is active: number of LEDs currently in the Main Path) |
|                   | nled_aux  |  Internal parameter (when Pitlane is active: number of LEDs currently in the Pitlane Path)|
|                   | nled_init_aux  | Internal parameter (position of the Pitlane entrance)  |
|                   | **box_len**  | Total number of LEDs, at the end of the Racetrack, reserved for the Pitlane (**configurable with "B" command**)    |
| **RAMP** params  |   |   |
|| **start** |   LED number where the ramp Starts
|| **center** |   LED Number where ramp is centered.
|| **end** |   LED number where the ramp ends
|| **height**  | Ramp elevation
| **RACE** params  |   |   |
|| **start** | OLRNetwork Only **Always 1 for standalone OLR**
|| **nlap** |Number of laps of a Race (**Configurable with 2nd parameter of "C" Command**)
|| **repeat**  | OLRNetwork Only **Always 1 for standalone OLR**
|| **finish** | OLRNetwork Only **Always 1 for standalone OLR**





#### Examples
|Origin|Command||
|---|----|----|
|H|**Q[EOC]**| Host send a **get current paremeters Set** request
|B|**TRACK:1200,1200,0,-1,60[EOC]<br>RAMP:180,190,200,15[EOC]<br>RACE:1,2,1,1[EOC]<br>**|Messages from the Board with the current cfg values




- - -


### [**p**] - Current Car **p**osition in Race
|initiate| Syntax | Description
|------|--------|------------
|B|**p**NumTrackNlapRpos[EOC]|**Position for each car in the race**
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
|B|**p**1B1.95**p**2M5.45[EOC]| Two cars are currentry "running" in the Board. Car "1" is in Track "B" in Lap number "1" Relative Lap Position 95%. Car "2" is in Track "M" in Lap number "5" Relative Lap Position 45%


- - -

### [w] - Car Win te Race

|initiate| Syntax | Description
|------|--------|------------
|B|**w**Num[EOC]|**Car 'Num' just win the race**
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




- - -

**Revisions:**
- 2020 07 28
	- Command T: Syntax  mofied 	
	- Command A: Syntax  mofied 	
	- Command B: Added
	-

- - -
