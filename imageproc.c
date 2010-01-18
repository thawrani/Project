#include "approach002.h"
#define SUCCESS 0
#define EXIT 4


int process(image *,image *,char **);
int median(image* ,image* ,char** );
RGBTRIPLE proc(RGBTRIPLE** ,int ,int ,int);
RGBTRIPLE mediansort(RGBTRIPLE** ,RGBTRIPLE* ,int );
int convgs(image*,image*,char**);
int averaging(image*,image*,char**);
RGBTRIPLE proc2(RGBTRIPLE** ,int ,int ,int );


int process(image* inImage,image* outImage,char** argv)
{
  int choice;
  int err;

  do
    {
      printf("\n1.Convert To Grey Scale\n2.Median Filter\n3.Averaging Filter\n4.Exit\n");
      printf("Choice:");
      scanf("%d",&choice);
      switch(choice)
	{
	  
	case 1:
	    if((err=convgs(inImage,outImage,argv))!=SUCCESS)
	      {
		printf("Error Encountered\n");
		exit(1);
	      }
	    break;
	case 2:
	  if((err=median(inImage,outImage,argv))!=SUCCESS)
	    {
	      printf("Error Encountered\n");	      
	      exit(1);
	    }
	  break;
	case 3:
	  if((err=averaging(inImage,outImage,argv))!=SUCCESS)
	    {
	      printf("Error Encountered\n");
	      exit(1);
	    }
	      break;
	default:
	  if(choice==4)
	    return EXIT;
	  else
	    printf("Re-enter Choice\n");
	}
    }while(choice!=4);
 
 return SUCCESS;
}

int averaging(image* inImage,image* outImage,char** argv)
{
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmih;
  RGBTRIPLE pixel;
  int i,j,err,masksize;
  FILE* fp1;
  FILE* fp2;
  RGBTRIPLE** imRawData;

  fp1=fopen(argv[1],"rb");
  fp2=fopen(argv[2],"wb");

  printf("Enter mask size:");
  scanf("%d",&masksize);

  inImage->imName=(BYTE *)argv[1];
  outImage->imName=(BYTE *)argv[2];
  
  fread((BYTE *)&bmfh,sizeof(BITMAPFILEHEADER),1,fp1);
  inImage->imType=outImage->imType=bmfh.bfSize;
  inImage->imOffset=outImage->imOffset=bmfh.bfOffsetBits;
  printf("%x\n",bmfh.bfType);

  fread((BYTE *)&bmih,sizeof(BITMAPINFOHEADER),1,fp1);
  inImage->imSize=bmfh.bfSize;
  inImage->imWidth=outImage->imWidth=bmih.biWidth;
  inImage->imHeight=outImage->imHeight=bmih.biHeight;
  inImage->imBitsPerPixel=outImage->imBitsPerPixel=bmih.biBitCount;
  outImage->imSize=inImage->imWidth * inImage->imHeight ;
  

  printf("%ld\n",bmih.biWidth);

  fwrite((BYTE *)&bmfh,sizeof(BITMAPFILEHEADER),1,fp2);
  fwrite((BYTE *)&bmih,sizeof(BITMAPINFOHEADER),1,fp2);

  fseek(fp1,0,SEEK_SET+(inImage->imOffset));
  fseek(fp2,0,SEEK_SET+(outImage->imOffset));

  imRawData=(RGBTRIPLE **)malloc(((inImage->imHeight)+2) * sizeof(RGBTRIPLE *));
  
  
  
  for(i=0;i<( (inImage->imHeight) +2);i++)
    {
      imRawData[i]=(RGBTRIPLE *)malloc(((inImage->imWidth)+2) * sizeof(RGBTRIPLE));
    }
 

  for(i=0;i<( (inImage->imHeight) + 2 );i++)
    {
      for(j=0;j<((inImage->imWidth)+2);j++)
	{    
	  if(((i==0 || i==((inImage->imHeight)+1))))
	    {
	      imRawData[i][j].rgbtRed=imRawData[i][j].rgbtGreen=imRawData[i][j].rgbtBlue=0;
	    }
	  else if(((j==0 || j==((inImage->imWidth)+1))))
	    {
	      imRawData[i][j].rgbtRed=imRawData[i][j].rgbtGreen=imRawData[i][j].rgbtBlue=0;
	    }
	  else
	    {
	      fread((BYTE *)&pixel,1,sizeof(RGBTRIPLE),fp1);
	      (imRawData)[i][j]=pixel;
	    }

	}
    }
  for(i=1;i<=(inImage->imHeight);i++)
    {
      for(j=1;j<=(inImage->imWidth);j++)
	{
	  pixel= proc2(imRawData,i,j,masksize);
	  fwrite((BYTE *)&pixel,1,sizeof(RGBTRIPLE),fp2);
	}
    }
  for(i=0;i<((inImage->imHeight) + 2);i++)
    {
      free((imRawData)[i]);
    }
  free((imRawData));
  fclose(fp1);
  fclose(fp2);
  return SUCCESS;
}



RGBTRIPLE proc2(RGBTRIPLE** imData,int rowNo,int colNo,int masksize)
{
 
  RGBTRIPLE** MASK;
  int x,y,count;
  RGBTRIPLE average;
  int Red=0;
  int Green=0;
  int Blue=0;
  
  MASK=(RGBTRIPLE **)malloc(masksize * sizeof(RGBTRIPLE *));

  for(x=0;x<=(masksize-1);x++)
    {
      MASK[x]=(RGBTRIPLE *)malloc(masksize * sizeof(RGBTRIPLE));
    }
  for(x = 0 ; x <= ( masksize -1 ) ; x++)
    {
      for(y = 0 ; y <= ( masksize - 1 ) ; y++)
	{
	  MASK[x][y]=imData[rowNo+(x-(int)(masksize/2))][colNo+(y-(int)(masksize/2))];
	  Red=Red + MASK[x][y].rgbtRed;
	  Green=Green + MASK[x][y].rgbtGreen;
	  Blue=Blue + MASK[x][y].rgbtBlue;
	}
    }
  Red=Red / (pow(masksize,2));
  Green=Green / (pow(masksize,2));
  Blue=Blue / (pow(masksize,2));
    if(Red>=255)
    Red=255;
  if(Red<=0)
    Red=0;
  if(Green>=255)
    Green=255;
  if(Green<=0)
    Green=0;
  if(Blue>=255)
    Blue=255;
  if(Blue<=0)
    Blue=0x00; 
  average.rgbtRed=(BYTE)(Red);
  average.rgbtGreen=(BYTE)(Green);
  average.rgbtBlue=(BYTE)(Blue);

  for(x=0;x<(masksize);x++)
    free(MASK[x]);

  free(MASK);
  return average;
}

int median(image* inImage,image* outImage,char** argv)
{
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmih;
  RGBTRIPLE pixel;
  int i,j,err,masksize;
  FILE* fp1;
  FILE* fp2;
  RGBTRIPLE** imRawData;

  fp1=fopen(argv[1],"rb");
  fp2=fopen(argv[2],"wb");

  printf("Enter mask size:");
  scanf("%d",&masksize);

  inImage->imName=(BYTE *)argv[1];
  outImage->imName=(BYTE *)argv[2];
  
  fread((BYTE *)&bmfh,sizeof(BITMAPFILEHEADER),1,fp1);
  inImage->imType=outImage->imType=bmfh.bfSize;
  inImage->imOffset=outImage->imOffset=bmfh.bfOffsetBits;
  printf("%x\n",bmfh.bfType);

  fread((BYTE *)&bmih,sizeof(BITMAPINFOHEADER),1,fp1);
  inImage->imSize=bmfh.bfSize;
  inImage->imWidth=outImage->imWidth=bmih.biWidth;
  inImage->imHeight=outImage->imHeight=bmih.biHeight;
  inImage->imBitsPerPixel=outImage->imBitsPerPixel=bmih.biBitCount;
  outImage->imSize=inImage->imWidth * inImage->imHeight ;
  

  printf("%ld\n",bmih.biWidth);

  fwrite((BYTE *)&bmfh,sizeof(BITMAPFILEHEADER),1,fp2);
  fwrite((BYTE *)&bmih,sizeof(BITMAPINFOHEADER),1,fp2);

  fseek(fp1,0,SEEK_SET+(inImage->imOffset));
  fseek(fp2,0,SEEK_SET+(outImage->imOffset));

  imRawData=(RGBTRIPLE **)malloc(((inImage->imHeight)+2) * sizeof(RGBTRIPLE *));
  
  
  
  for(i=0;i<=( (inImage->imHeight) +2);i++)
    {
      imRawData[i]=(RGBTRIPLE *)malloc(((inImage->imWidth)+2) * sizeof(RGBTRIPLE));
    }
 

  for(i=0;i<=( (inImage->imHeight) + 2 );i++)
    {
      for(j=0;j<=((inImage->imWidth)+2);j++)
	{    
	  if(((i==0 || i==((inImage->imHeight)+2))))
	    {
	      imRawData[i][j].rgbtRed=imRawData[i][j].rgbtGreen=imRawData[i][j].rgbtBlue=0;
	    }
	  else if(((j==0 || j==((inImage->imWidth)+2))))
	    {
	      imRawData[i][j].rgbtRed=imRawData[i][j].rgbtGreen=imRawData[i][j].rgbtBlue=0;
	    }
	  else
	    {
	      fread((BYTE *)&pixel,1,sizeof(RGBTRIPLE),fp1);
	      (imRawData)[i][j]=pixel;
	    }

	}
    }
  for(i=1;i<=(inImage->imHeight)+1;i++)
    {
      for(j=1;j<=(inImage->imWidth)+1;j++)
	{
	  pixel= proc(imRawData,i,j,masksize);
	  fwrite((BYTE *)&pixel,1,sizeof(RGBTRIPLE),fp2);
	}
    }
  for(i=0;i<((inImage->imHeight) + 2);i++)
    {
      free((imRawData)[i]);
    }
  free((imRawData));
  fclose(fp1);
  fclose(fp2);
  return SUCCESS;
}

RGBTRIPLE proc(RGBTRIPLE** imData,int rowNo,int colNo,int masksize)
{
 
  RGBTRIPLE** MASK;
  RGBTRIPLE* sort;
  RGBTRIPLE pixel;
  int x,y,count;


  
  MASK=(RGBTRIPLE **)malloc(masksize * sizeof(RGBTRIPLE *));

  sort=(RGBTRIPLE *)malloc( (masksize*masksize) * sizeof(RGBTRIPLE));

  for(x=0;x<=(masksize-1);x++)
    {
      MASK[x]=(RGBTRIPLE *)malloc(masksize * sizeof(RGBTRIPLE));
    }
  for(x = 0 ; x <= ( masksize -1 ) ; x++)
    {
      for(y = 0 ; y <= ( masksize - 1 ) ; y++)
	{
	  MASK[x][y]=imData[rowNo+(x-(int)(masksize/2))][colNo+(y-(int)(masksize/2))];
	}
    }
  pixel=mediansort(MASK,sort,masksize);
  for(x=0;x<(masksize);x++)
    free(MASK[x]);

  free(MASK);
  free(sort);
  return pixel;
}

RGBTRIPLE mediansort(RGBTRIPLE** MASK,RGBTRIPLE* sort,int masksize)
{
  RGBTRIPLE medianvalue;
  RGBTRIPLE temp;
  int x,y,count;
  count =0;
  
  
  for(x=0;x<(masksize);x++)
    {
      for(y=0;y<(masksize);y++)
	{
	  sort[count++]=MASK[x][y];
	}
    }
  
  for(x=0;x<(masksize*masksize);x++)
    {
      for(y=((masksize * masksize)-1);y>x;y--)
	{
	  if(sort[y-1].rgbtRed>sort[y].rgbtRed)
	    {
	      temp.rgbtRed=sort[y-1].rgbtRed;
	      sort[y-1].rgbtRed=sort[y].rgbtRed;
	      sort[y].rgbtRed=temp.rgbtRed;
	    }
	  if(sort[y-1].rgbtGreen>sort[y].rgbtGreen)
	    {
	      temp.rgbtGreen=sort[y-1].rgbtGreen;
	      sort[y-1].rgbtGreen=sort[y].rgbtGreen;
	      sort[y].rgbtGreen=temp.rgbtGreen;
	    }
	  if(sort[y-1].rgbtBlue>sort[y].rgbtBlue)
	    {
	      temp.rgbtBlue=sort[y-1].rgbtBlue;
	      sort[y-1].rgbtBlue=sort[y].rgbtBlue;
	      sort[y].rgbtBlue=temp.rgbtBlue;
	    }
	}
    }
  
  medianvalue=sort[(int)((masksize*masksize)/2)];
  
  return medianvalue;
}

  
int convgs(image* inImage,image* outImage,char** argv)
{
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmih;
  RGBTRIPLE pixel;
  
  int i,j,err;
  FILE* fp1;
  FILE* fp2;
  fp1=fopen(argv[1],"rb");
  fp2=fopen(argv[2],"wb");
  

  inImage->imName=(BYTE *)argv[1];
  outImage->imName=(BYTE *)argv[2];
  
  fread((BYTE *)&bmfh,sizeof(BITMAPFILEHEADER),1,fp1);
  inImage->imType=outImage->imType=bmfh.bfSize;
  inImage->imOffset=outImage->imOffset=bmfh.bfOffsetBits;
  printf("%x\n",bmfh.bfType);

  fread((BYTE *)&bmih,sizeof(BITMAPINFOHEADER),1,fp1);
  inImage->imSize=bmfh.bfSize;
  inImage->imWidth=outImage->imWidth=bmih.biWidth;
  inImage->imHeight=outImage->imHeight=bmih.biHeight;
  inImage->imBitsPerPixel=outImage->imBitsPerPixel=bmih.biBitCount;
  outImage->imSize=inImage->imWidth * inImage->imHeight ;
  

  printf("%ld\n",bmih.biWidth);

  fwrite((BYTE *)&bmfh,sizeof(BITMAPFILEHEADER),1,fp2);
  fwrite((BYTE *)&bmih,sizeof(BITMAPINFOHEADER),1,fp2);

  fseek(fp1,0,SEEK_SET+(inImage->imOffset));
  fseek(fp2,0,SEEK_SET+(outImage->imOffset));
  for(i=0;i<inImage->imHeight;i++)
    {
      for(j=0;j<inImage->imWidth;j++)
	{
	  fread((BYTE *)&pixel,1,sizeof(RGBTRIPLE),fp1);
	  pixel.rgbtBlue=pixel.rgbtRed=pixel.rgbtGreen=(BYTE)(((0.30 * (int)pixel.rgbtRed) + (0.59 * (int)pixel.rgbtGreen) + (0.11 * (int)pixel.rgbtBlue))/3);
	  fwrite((BYTE *)&pixel,1,sizeof(RGBTRIPLE),fp2);
	}
    }
  fclose(fp1);
  fclose(fp2);
  return SUCCESS;
}

  
int main(int argc,char **argv)
{
  image inImage;
  image outImage;
  int err;
  

  
  if(argc!=3)
    {
      printf("Insufficient arguments\nUsage:./progname infile\n");
      exit(1);
    }

 
  
  if((err=process(&inImage,&outImage,argv))!=SUCCESS)
  {
    if(err!=EXIT)
      printf("\nError\n");
    exit(1);
  }
  
  return SUCCESS;
}





      

