# Instructions for Setup

Follow following instructions to setup the project.

1. Clone the following repository on your lab[1-5] machine
   `git clone https://github.com/cu-scalab/Project-1`
2. Go to the code directory
3. Run the following command:- 
   `./ert config.txt`
  Step 3 will run the tool and produce results. It will save the results in Results.lab.cs.clemson.edu folder. It will produce a .ps file. 
4. Convert it to a pdf file using following command:-
 `ps2pdf roofline.ps roofline.pdf`
5. Download it to your local system using scp
   'scp username@lab1.cs.clemson.edu:/your_project_directory/.ps file'
   
# Questions

The pdf file contains graph. Refer to this graph and answer the following questions:-

1. What does FLOPs/Byte mean?
2. Why are there two purple lines and what are the values?
3. What is the blue line? Why is it flat?
4. In simple terms, what does this graph tell us about lab[1-5]?

# Submission
 Your submission should be a pdf report containing answer to the questions mentioned above.
