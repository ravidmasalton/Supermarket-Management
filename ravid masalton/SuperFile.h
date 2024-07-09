#pragma once
typedef unsigned char BYTE;

#include <stdio.h>
#include "Supermarket.h"

int	saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName, const char* fileNameCompressed, const char* customersFileName);
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName, int indication,const char* customersFileName);

//int		loadCustomerFromTextFile(SuperMarket* pMarket, const char* customersFileName);


int	loadProductFromTextFile(SuperMarket* pMarket, const char* fileName);
int readSuperFromBinaryFileCompressed(SuperMarket* pMarket, const char* fileName);
int readAddessrFromBinaryFileCompressed(SuperMarket* pMarket, FILE* fp);
Product* readProductrFromBinaryFileCompressed(SuperMarket* pMarket, FILE* fp);
int readBarcode(SuperMarket* pMarket, FILE* fp, char* barcode, BYTE data[]);
int writeAllProductToBinaryFileCompressed(SuperMarket* pMarket, FILE* fp);
int writeSuperToBinaryFileCompressed(SuperMarket* pMarket, const char* fileName);
int writeAddressToBinaryFileCompressed(SuperMarket* pMarket, FILE* fp);
int writeAllProductToBinaryFileCompressed(SuperMarket* pMarket, FILE* fp);
int writeProductToBinaryFileCompressed(Product* p, FILE* fp);
char castingToRead(char ch);
char castingTowrite(char ch);
