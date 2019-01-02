# Trains and Deadlock detection

### Overview
This assignment was designed to mimic trains meeting at railway junction. Each train could come from
either the north, west, south or east. For a given train to pass through the junction it had to acquire 3
locks. One for the direction it was coming from , second the lock for the train to its right N→W→S→E
back to N. And finally it had to have the junction lock it self. Once all three locks were acquired it
could easily pass through the junction. In this case the case the junction was a 2 second sleep function.

The number of trains was read in from a text file called sequence.txt. This file has a giant string of the
total number of trains that would be arriving at that instance, denoted by either N,W, S, or E. Then after
this file was read in and n trains were created for the amount for the amount of trains in the string in
sequence.txt. At the time of the train spawning they were given a probability they would spawn and if
they were below the threshold then it would try again on the next iteration of the while loop. This was
to give some randomization to how the trains arrived, and again trying to mimic real life.

As said before the trains would require the three locks needed to cross the junction. Each time a train
made a request = 1 , acquired = 2, or released/no request =0 a lock was placed on the a matrix.txt. This
file acted as the shared memory between each process. Everytime the matrix file updated a lock was
used to update it, to make sure nothing was missed.

Prior to updating the text file would be read in to update the processes local matrix, then the new 1,2,0
was placed in the matrix then the local matrix would update the matrix.txt. The reason this method was
to used to keep in line RAG deadlock detection. This matrix.txt would allow to see if cycles are
formed between the trains. A cycle could occur if al N,W,S,E train had its respective lock ( = 2 on the
matrix.txt) and was requesting the train to its rights lock ( = 1 on the matrix.txt). If there is a cycle the
trains(program) would deadlock.

Lastly a method for deadlock detection was developed to see if indeed a cycle had occurred or was just
about too. The method involved locking out the matrix.txt, and putting all the contents of each matrix
column its own array. Then it would check each array for any 2’s. If was one in each array and they
were not located to the in the same index for the array to its right (NW,WS,SE,EN) then there was a
deadlock. Basically if your were iterating through the arrays the same i-th value put in each array
should only yield one 2 value or else no alarms should be raised. If four 2’s on different i-th values
arise then alarms should be raised. From this if deadlock was found the cycle with trains direction and
pid would be printed along with matrix it self at that point.

### Conclusion
It was was found that no one opposing sides can never deadlock one another. Second it was found that
the longer the sequence of trains that contains multi trains with at least one of each kind of train, the
greater the risk of deadlocking. My method to run deadlock detection was to run n + 1 process. This
one extra process only job was to check for constant deadlocking. It would stop the program to report if
it had occurred or just run until the process was killed by the parent process.

### Authors
Tye Borden

### Acknoledgements 
Project from Saint Mary's OS course 2018
