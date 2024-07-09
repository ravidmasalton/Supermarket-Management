#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_Macros.h"
#include "Address.h"
#include "General.h"
#include "fileHelper.h"
#include "SuperFile.h"



int	saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName, const char* fileNameCompressed,const char* customersFileName)
{ 

	
		if (!writeSuperToBinaryFileCompressed(pMarket, fileNameCompressed))
			return 0;

	
		
		  FILE* fp;
		  fp = fopen(fileName, "wb");
		  CHECK_MSG_RETURN_0(fp, "Error open supermarket file to write\n");

		  if (!writeStringToFile(pMarket->name, fp, "Error write supermarket name\n"))
		  {
			  CLOSE_RETURN_0(fp);
		  }

		  if (!saveAddressToFile(&pMarket->location, fp))
		  {
			  CLOSE_RETURN_0(fp);
		  }
		  int count = getNumOfProductsInList(pMarket);

		  if (!writeIntToFile(count, fp, "Error write product count\n"))
		  {
			  CLOSE_RETURN_0(fp);
		  }

		  Product* pTemp;
		  NODE* pN = pMarket->productList.head.next; //first Node
		  while (pN != NULL)
		  {
			  pTemp = (Product*)pN->key;
			  if (!saveProductToFile(pTemp, fp))
			  {
				  CLOSE_RETURN_0(fp);
			  }
			  pN = pN->next;
		  }

	     fclose(fp);
	
	saveCustomerToTextFile(pMarket->customerArr,pMarket->customerCount, customersFileName);
	return 1;
}
#define BIN
#ifdef BIN
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName, int indication,const char* customersFileName)
{      
	
	if (indication)
	{
		if (!readSuperFromBinaryFileCompressed(pMarket, fileName))
			return 0;
		
			pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
			if (!pMarket->customerArr)
				return 0;
			printf(" successfully reading to filesCompressed\n");
			return	1;
		
	}
	
		FILE* fp;
		fp = fopen(fileName, "rb");
		CHECK_MSG_RETURN_0(fp, "Error open company file\n");

		pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
		if (!pMarket->name)
		{
			FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);
		}

		if (!loadAddressFromFile(&pMarket->location, fp))
		{
			FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);
		}

		int count;
		if (!readIntFromFile(&count, fp, "Error reading product count\n"))
		{
			FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);
		}

		if (count > 0)
		{
			Product* pTemp;
			for (int i = 0; i < count; i++)
			{

				pTemp = (Product*)calloc(1, sizeof(Product));
				if (!pTemp)
				{

					printf("Allocation error\n");
					L_free(&pMarket->productList, freeProduct);
					FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);
				}
				if (!loadProductFromFile(pTemp, fp))
				{

					L_free(&pMarket->productList, freeProduct);
					FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);
				}
				if (!insertNewProductToList(&pMarket->productList, pTemp))
				{
					L_free(&pMarket->productList, freeProduct);
					FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);
				}
			}
		}
		fclose(fp);

	pMarket->customerArr = loadCustomerFromTextFile(customersFileName,&pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;
	return	1;

}
#else
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	if (!fp)
	{
		printf("Error open company file\n");
		return 0;
	}

	//L_init(&pMarket->productList);


	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		fclose(fp);
		return 0;
	}

	if (!loadAddressFromFile(&pMarket->location, fp))
	{
		free(pMarket->name);
		fclose(fp);
		return 0;
	}

	fclose(fp);

	loadProductFromTextFile(pMarket, "Products.txt");


	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;

}
#endif

int	 loadProductFromTextFile(SuperMarket* pMarket, const char* fileName)
{
	FILE* fp;
	//L_init(&pMarket->productList);
	fp = fopen(fileName, "r");
	int count;
	fscanf(fp, "%d\n", &count);


	//Product p;
	Product* pTemp;
	for (int i = 0; i < count; i++)
	{
		pTemp = (Product*)calloc(1, sizeof(Product));
		if (pTemp == NULL)
			return 0;
		myGets(pTemp->name, sizeof(pTemp->name), fp);
		myGets(pTemp->barcode, sizeof(pTemp->barcode), fp);
		fscanf(fp, "%d %f %d\n", &pTemp->type, &pTemp->price, &pTemp->count);
		insertNewProductToList(&pMarket->productList, pTemp);
	}

	fclose(fp);
	return 1;
}

int writeSuperToBinaryFileCompressed(SuperMarket* pMarket, const char* fileName)
{

	FILE* fp = fopen(fileName, "wb");
	if (!fp)
		return 0;

	int numOfProduct = getNumOfProductsInList(pMarket);
	int lengthName = (int)strlen(pMarket->name);

	BYTE data[2] = { 0 };
	data[0] = numOfProduct >> 2;
	data[1] = ((numOfProduct &0x3) << 6) | lengthName;

	if (fwrite(data, sizeof(BYTE), 2, fp) != 2)
		CHECK_MSG_RETURN_0(fp, "Error writing supermarket\n");

	if (!writeCharsToFile(pMarket->name, lengthName, fp, "Error writing supermarket name\n"))
	{
		CLOSE_RETURN_0(fp);
	}
	if(!writeAddressToBinaryFileCompressed(pMarket,fp))
		CLOSE_RETURN_0(fp);

	if (!writeAllProductToBinaryFileCompressed(pMarket, fp))
		return 0;

	fclose(fp);

	return 1;
}
int writeAddressToBinaryFileCompressed(SuperMarket* pMarket,FILE*fp)
{
	BYTE data;
	data = pMarket->location.num;
	if (fwrite(&data, sizeof(BYTE), 1, fp) != 1)
		CHECK_MSG_RETURN_0(fp, "Error writing supermarket\n");

	if(!writeStringToFile2(pMarket->location.street, fp, "Error writing supermarket street\n"))
		CLOSE_RETURN_0(fp);

	if(!writeStringToFile2(pMarket->location.city, fp, "Error writing supermarket cuty\n"))
		CLOSE_RETURN_0(fp);
	return 1;
}

int writeAllProductToBinaryFileCompressed(SuperMarket* pMarket, FILE* fp)
{
	
	NODE* curr = pMarket->productList.head.next;
	while (curr!=NULL)
	{
		Product* p = (Product*)curr->key;
		if(!writeProductToBinaryFileCompressed(p,fp))
			CLOSE_RETURN_0(fp);

		curr = curr->next;

	}
	return 1;
}

int writeProductToBinaryFileCompressed(Product* p, FILE* fp)
{
	BYTE data1[3];
	
	char barcode[BARCODE_LENGTH + 1];
	strcpy(barcode, p->barcode);



	char ch=barcode[0];
	int i = 0;
	while (ch != '\0')
	{
		barcode[i] = castingTowrite(ch);
		i++;
		ch = barcode[i];
	}


	data1[0] = barcode[0] << 2 | (barcode[1] >> 4);
	data1[1] = ((barcode[1] & 0x0f) << 4) | (barcode[2] >> 2);
	data1[2] = ((barcode[2] & 0x03) << 6) | barcode[3];
	if (fwrite(data1, sizeof(BYTE), 3, fp) != 3)
		return 0;
	BYTE name = (BYTE)strlen(p->name);

	BYTE data2[3];

	data2[0] = ((barcode[4] << 2) | barcode[5] >> 4);
	data2[1] = ((barcode[5] & 0x0f) << 4) | (barcode[6] >> 2);
	data2[2] = ((barcode[6] & 0x3) << 6) | ((name << 2) | p->type);

	if (fwrite(data2, sizeof(BYTE), 3, fp) != 3)
		CLOSE_RETURN_0(fp);

	if (!writeCharsToFile(p->name, (int)strlen(p->name), fp, "Error writing supermarket street\n"))
		CLOSE_RETURN_0(fp);

	BYTE data3[3];
	data3[0] = p->count;

	int shakel =(int) p->price;
	int agorot = (int)((p->price - shakel) * 100);

	data3[1] = agorot << 1 | (shakel >> 8);
	data3[2] = shakel & 0x0ff;

	if (fwrite(data3, sizeof(BYTE), 3, fp) != 3)
		CLOSE_RETURN_0(fp);

	return 1;
}




int readSuperFromBinaryFileCompressed(SuperMarket* pMarket, const char* fileName)
{
	FILE* fp = fopen(fileName, "rb");
	if (!fp)
		return 0;
	
	BYTE data[2];
	
	if (fread(data, sizeof(BYTE), 2, fp) != 2)
		return 0;
	
	int numOfProduct = data[0] <<2 | data[1] >> 6;
	int lengthName = data[1] & 0x3f;

	pMarket->name = (char*)calloc(lengthName + 1, sizeof(char));
	CHECK_RETURN_0(pMarket->name);

	if (fread(pMarket->name, sizeof(char), lengthName, fp) != lengthName)
	{
		FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);
	}
	
	if (!readAddessrFromBinaryFileCompressed(pMarket, fp))
	{
		FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);
	}
	
	for (int i = 0; i < numOfProduct; i++)
	{
		Product* p = readProductrFromBinaryFileCompressed(pMarket, fp);
		if (!p)
		{
			freeAddress(&pMarket->location);
			FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);

		}
		
		if(!insertNewProductToList(&pMarket->productList, p))
		{
			freeAddress(&pMarket->location);
			FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);
		}

	}
	
	pMarket->sortOpt = 0;
	fclose(fp);
	return 1;

}

int readAddessrFromBinaryFileCompressed(SuperMarket* pMarket, FILE* fp)
{
	
	BYTE data;
	if (fread(&data, sizeof(BYTE), 1, fp) != 1)
		return 0;
	pMarket->location.num = data;
	
	pMarket->location.street = readStringFromFile2(fp, "Error reading supermarket street\n");
	

	CHECK_RETURN_0(pMarket->location.street);
	
	pMarket->location.city = readStringFromFile2(fp, "Error reading supermarket city\n");
	CHECK_RETURN_0(pMarket->location.city);
		return 1;
		
}

Product* readProductrFromBinaryFileCompressed(SuperMarket* pMarket, FILE* fp)
{
	BYTE data[3];
	char barcode[8];
	if (!readBarcode(pMarket, fp, barcode, data))
	{
		freeAddress(&pMarket->location);
		fclose(fp);
	}

	Product* product = (Product*)malloc(sizeof(Product));
	CHECK_RETURN_0(product);

	
	strcpy(product->barcode, barcode);

	product->type = (data[2] & 0x3);
	int lengthName = (data[2] & 0x3c)>>2;
	if (fread(product->name, sizeof(char), lengthName, fp) != lengthName)
	{
		fclose(fp);
		return NULL;
	}
	
	product->name[lengthName] = '\0';

	if (fread(data, sizeof(BYTE), 3, fp) != 3)
		return NULL;
	
	product->count = data[0];
	int agorot =  data[1] >> 1;
	int shakl =(data[1] &0x1)<<8 | (data[2]);
	float f = (float)shakl + (float)(agorot / 100.0);
	product->price = f;
	
	return product;
}

int readBarcode(SuperMarket* pMarket, FILE* fp,char*barcode, BYTE data[])
{
	if (fread(data, sizeof(BYTE), 3, fp) != 3)
		return 0;

	barcode[0] = (data[0] >> 2);
	barcode[0] = castingToRead(barcode[0]);

	barcode[1] = (data[0] & 0x3) << 4 | (data[1] >> 4);
	barcode[1] = castingToRead(barcode[1]);

	barcode[2] = (data[1] & 0x0f) << 2 | (data[2] >> 6);
	barcode[2] = castingToRead(barcode[2]);

	barcode[3] = (data[2] & 0x3f);
	barcode[3] = castingToRead(barcode[3]);


	if (fread(data, sizeof(BYTE), 3, fp) != 3)
		return 0;

	barcode[4] = (data[0] >> 2);
	barcode[4] = castingToRead(barcode[4]);

	barcode[5] = (data[0] & 0x3) << 4 | (data[1] >> 4);
	barcode[5] = castingToRead(barcode[5]);

	barcode[6] = ((data[1] & 0x0f) << 2) | ((data[2] & 0xc0) >> 6);
	barcode[6] = castingToRead(barcode[6]);
	barcode[7] = '\0';

	return 1;
}

char castingToRead(char ch)
{
	if (ch >= 10)
	{
		ch += 55;
	}
	else
		ch += 48;
	return ch;

}


char castingTowrite(char ch)
{
	if (ch >= 'A' && ch<='Z')
	{
		ch -= 55;
	}
	else
		ch -= 48;
	return ch;

}