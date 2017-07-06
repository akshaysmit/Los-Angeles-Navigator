Created in March, 2017. Average search time < 0.18 seconds on UCLA's lnxsrv07 and lnxsrv09. There are no known issues.

A navigation software which uses the A* algorithm (with great-circle distance on the Earth as a heuristic) to give street-by-street navigation instructions between
two locations in Los Angeles. Technically, the graph can be expanded to include other cities as well. This can be done by inserting locations into the mapdata file. 
A list of valid locations is contained inside the validlocs file. 

The exact command line usage instructions are at the beginning of main.cpp .  

AttractionMapper and SegmentMapper both use a binary search tree which has been implemented in MyMap.h . The A* algorithm also uses this binary search tree, along 
with an STL priority queue.

To see the big-O complexity of various important functions, see report.docx .
