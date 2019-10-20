# Graph Cut

This is an implementation of the classical graph cut algorithm. The calculation of max flow is also implemented by myself, using the Ford Fulkerson algorithm. Although the results obtained are comparable to others, the running time has yet to be optimized. A picture of about 200*200 pixels takes about 1 minutes to calculate, and most of the time is consumed in calculating the max flow. 

### Usage 

In the x64 folder, there are two executable files, through them, you can run the program directly (require OpenCV DLL file in your system path).

In the source code folder, there is a .txt file named "img_name.txt". Type the name of image you want to test and save the file. Run the program.

After the 'Display Window' is showed, press the left mouse button to draw object labels ( they should be blue lines), press both 'ALT' key and the left button to draw background labels (green lines).

If you have finished the labeling job, please press the space key to run the graph cut algorithm.  

When new windows pops up, the graph cut is completed. Press "s" to store the output images.  

Press 'Esc' key to exit the program.

**IMPORTANT:** Please use release mode !

### Compile

**IDE**: Visual Studio 2019

**3rd party library required:**  OpenCV 4.1.1

### Results

<figure class="half">     
      <img src="./GraphCut/images/carsten.jpg">     
      <img src="./GraphCut/images/carsten_drawed.jpg"> 
</figure>  

<figure class="half">     
      <img src="./GraphCut/images/carsten_out.jpg">     
      <img src="./GraphCut/images/carsten_mask.jpg"> 
</figure>  



<figure class="half">     
      <img src="./GraphCut/images/hat.jpg">     
      <img src="./GraphCut/images/hat_drawed.jpg"> 
</figure>  

<figure class="half">     
      <img src="./GraphCut/images/hat_out.jpg">     
      <img src="./GraphCut/images/hat_mask.jpg"> 
</figure>  



<figure class="half">     
      <img src="./GraphCut/images/tree.jpg">     
      <img src="./GraphCut/images/tree_drawed.jpg"> 
</figure>  

<figure class="half">     
      <img src="./GraphCut/images/tree_out.jpg">     
      <img src="./GraphCut/images/tree_mask.jpg"> 
</figure>  



<figure class="half">     
      <img src="./GraphCut/images/horse.jpg">     
      <img src="./GraphCut/images/horse_drawed.jpg"> 
</figure> 

<figure class="half">     
      <img src="./GraphCut/images/horse_out.jpg">     
      <img src="./GraphCut/images/horse_mask.jpg"> 
</figure> 

<figure class="half">     
      <img src="./GraphCut/images/owls.jpg">     
      <img src="./GraphCut/images/owls_drawed.jpg"> 
</figure> 

<figure class="half">     
      <img src="./GraphCut/images/owls_out.jpg">     
      <img src="./GraphCut/images/owls_mask.jpg"> 
</figure> 

