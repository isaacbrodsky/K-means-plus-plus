Kmeansplusplus
==============

K-means implementation with optional k-means++ initialization, based on the k-means multi-attribute implementation
by John Aleshunas.

Requires C++11; no other dependencies. Compile by running `make` in the source directory.

Usage
=====

Run `k-means++ [control file name]`

Control file format
===================

The control file is a text file with labels followed by values.

```
#k-count <number of clusters, integer>
#input-filename <input data file, string>
#output-filename <output results file, string>
#use-labels <whether the data set contains labels, 0 or 1>
#tolerance <stopping tolerance, float>
#plus-plus <whether to use k-means++ initialization, 0 or 1>
#plus-plus-random-seed <random seed for k-means++ initialization, integer>
```

The control file is optionally terminated by a line containing `#EOF`. By default, k-means++ is enabled, and if no random seed is specified, the pseudo-random number generator will be seeded by the system random_device.

Data file format
================

The input data set begins with the number of attributes in the data set followed by the data in space separated format. For example,
```
4
-15.12250759 24.93993856 -5.30589478 9.997459102 class3
5.261768317 -9.97127782 29.27001687 -20.13308064 class2
0.208998924 -5.149476452 -6.093019773 13.73514169 class1
-15.72998313 24.42157466 -0.118093616 14.88103108 class3
```
is a dataset with 4 attributes. Each line contains the attributes for that data instance followed by, if #use-labels was set to 1 in the control file, the class of that data instance.

Results file format
===================

The results file is made up of blocks of text with headers like this:
```
Cluster #1 with mean -16.0961 26.8356 -0.453156 8.98302  and member count 83
```
Each of these blocks represents a cluser found by k-means. The numbers after the word "mean" are the mean (centroid) location of that cluster. The number of values in the mean will be the same as the number of attributes in the data set. The member count is number of data instances from the input data set that were clustered into this cluster.

Each header is then followed by lines like this:
```
-15.1225 24.9399 -5.30589 9.99746  class3
````
Each line represents a data instance from the input data set that was clustered into this cluster. The line is in the same format as the input data set.

External links
==============
http://mercury.webster.edu/aleshunas/Source%20Code%20and%20Executables/Source%20Code%20and%20Executables.html
