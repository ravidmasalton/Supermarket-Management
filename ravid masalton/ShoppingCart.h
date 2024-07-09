#pragma once

#include "ShoppingItem.h"
#define PRINT_DETAIL

#ifdef PRINT_DETAIL
#define  PRINT(price) printf("Total bill to pay: %.2f\n", price);
#else
#define PRINT(price) printf("");
#endif  //PRINT_DETAIL


typedef struct
{
	ShoppingItem**	itemArr;
	int				count;
}ShoppingCart;




void	initCart(ShoppingCart* pCart);
float	getTotalPrice(const ShoppingCart* pCart);
ShoppingItem*	getItemByBarocde(ShoppingCart* pCart, const char* barcode);
int		addItemToCart(ShoppingCart* pCart, const char* name, float price, int count);
float	printShoppingCart(const ShoppingCart* pCart);

void	freeShoppingCart(ShoppingCart* pCart);