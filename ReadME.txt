# Deadlock avoidance project

##Description
---
This project simulates having a certain amount of resources that are shared amongst N number of threads that are created at runtime. The program schedules them
in such a way that it avoids deadlock. For example suppose thread ONE needs 5 of resource A and has 3 and there's only 5 total of resource ONE, then thread one would sleep until 
it can get allocated all of the 5 total of that resource. but suppose Thread 2 also needs 5 of resource ONE and currently has 2 and sleeps. Neither of the threads can release
the resource until they meet their request and that would never happen so thus it DEADLOCKS. This program avoids these situations. 

##Instructions
---

(1) operating system: Windows 10 Pro 64-bit
(2) Tool used for running program: Cygwin64 Terminal
(3) How to run program:
	1.open up cygwin
	2. use 'cd' to navigate to folder with mainproject.cpp inside
	3. create an executable file using the line 'g++ 144project.cpp -o <nameofexecutible>.exe'
	4. now that exe file has been created run it by using './<nameofexecutible>.exe <numOfThreads> <numOfResources>
	note: max size possible for both is 10 and 10. 
	5. Program will now loop forever, to end program press 'Ctrl+C'
(4) largest (M N) program will run without a problem: 10 10

(5) VIDEOLINK: https://youtu.be/pvnt5xKHl5o

NOTE: Folder should contain a default exe file named '144project.exe' if there, you can skip step 3 of running program and run the 
default exe with './144project.exe <numOfThreads> <numOfResources>
