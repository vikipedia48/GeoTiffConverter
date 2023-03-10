# GeoTiffConverter
GUI application for converting GeoTiff images to a PNG format

# Functionality
The user can convert a GeoTiff image into a regular PNG image.

![geotiffconverterscreenshot1](https://user-images.githubusercontent.com/37978310/224421424-9f6f5e34-8944-4e30-a936-a372cc3aea0d.png)

# Methods of conversion
There are 5 different output modes.

Grayscale16_MinToMax - all values are mapped to grayscale values (0 - 65535), in which the minimum value becomes 0, the maximum becomes 65535, and all others are mapped to values between the minimum and the maximum; the output is a 16 bit grayscale image
![image](https://user-images.githubusercontent.com/37978310/224423081-1445816b-96ea-4276-89cc-414c850121f3.png)

Grayscale16_TrueValue - values contained within Tiff rasters are directly translated to a grayscale value (0 - 65535); user can specify an offset to be applied to all values; the output is a 16 bit grayscale image
![truevaluetest0screenshot](https://user-images.githubusercontent.com/37978310/224422668-e968f566-b853-4c41-8909-46d95aaa1b90.png)

RGB_UserValues - all values are directly converted to RGBA colors the user specifies; during the configuration of this mode, all distinct Tiff values are loaded, to which the user must assign a color; the output is a RNGA image
![image](https://user-images.githubusercontent.com/37978310/224427228-a8e9426d-b33e-45d5-a34f-650b73995641.png)

RGB_UserRanges - all values are converted to RGBA colors the user specifies; during configuration of this mode, the user can specify their own values and its adjoining colors; during conversion, Tiff values will be mapped to a user value and assigned the specified color; an option is available to use gradient colors, which will not directly assign the specified colors, but instead use a color between the upper and the lower range; the output is a RNGA image
![image](https://user-images.githubusercontent.com/37978310/224425006-ea35e0ab-288c-49ed-ba79-29909275b96e.png)
![image](https://user-images.githubusercontent.com/37978310/224425385-359c04ee-de05-4193-896e-f1c448295d95.png)

RGB_Formula - all values are converted to a RGBA color determined by a formula, which is : (R -> value div 65536, G -> (value - value div 65536) div 256, B -> value mod 256); if the value is negative, Alpha value of the pixel will be 128, otherwise 255; for example, 255 -> (0,0,255,255), 256 -> (0,1,0,255), 257 -> (0,1,1,255), 65535 -> (0,255,255,255), 65536 -> (1,0,0,255), -65537 -> (1,0,1,128); the output is a RNGA image
![image](https://user-images.githubusercontent.com/37978310/224425658-f80ba917-6fa6-4c1f-81d2-c4f60af43aa7.png)

# Cropping and tiling
Optional settings for outputting the image

The resulting image can be cropped by inputting the coordinates of the wanted upper left pixel and bottom right pixel

The resulting image can also be split into tiles (different files), whose size can be determined either by fixed size or by the fixed number of tiles 
![image](https://user-images.githubusercontent.com/37978310/224429206-c792552c-18a5-4e7b-aeab-47872f3036fe.png)
![image](https://user-images.githubusercontent.com/37978310/224430388-bb26c7db-b277-48c4-bf3d-01f2d415561e.png)
![image](https://user-images.githubusercontent.com/37978310/224430179-1fb7ed2a-2fe4-4799-b676-1a7318d4ea6f.png)
