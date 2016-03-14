/**
 * @file   tinyMathFunction.c
 * @brief  組み込み用高速数学関数 <br>
 *
 * @author RTCorp. Ryota Takahashi
 */


#include "type.h"
#include "tinyMathFunction.h"

#define INVSQRT_NEWTON_METHOD_NUM (2)

/**
* 高速なinvSqrt(x)の実装. <br>
* ニュートン法を一回実行するごとにfloatの乗算が3回必要 <br>
* INVSQRT_NEWTON_METHOD_NUMでニュートン法の適用回数を決める <br>
*  math.hのsqrt()に対して <br>
*  0:誤差は ±0.05以下 <br>
*  1:誤差は ±0.005以下 <br>
*  2:誤差は ±0.000005以下 <br>
*  3:誤差は ほぼなし <br>
* @param x 定義域は[-1.0,1.0]
* @return acosの値
*/
float tInvSqrt(float x) {
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float*)&i;
	//ニュートン法
	for (i = 0; i < INVSQRT_NEWTON_METHOD_NUM; i++)
	{
		y = y * (1.5f - (halfx * y * y));
	}
	return y;
}

/**
* 高速なsqrt(x)の実装. <br>
* invSqrt(x)*xでsqrt(x)を計算 <br>
* invSqrt(x)のニュートン法ステップ3回でほぼ誤差なしになる. <br>
*
* @param x 定義域は[-1.0,1.0]
* @return acosの値
*/
float tSqrt(float x)
{

	float halfx = 0.5f * x;
	float y = x;
	float val;

	if (x == 0.0f) return 0.0f;

	//invSqrt(y)を計算
	long i = *(long*)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float*)&i;
	//ニュートン法
	y = y * (1.5f - (halfx * y * y));
	y = y * (1.5f - (halfx * y * y));
	y = y * (1.5f - (halfx * y * y));
	//sqrtを計算
	val = x * y;
	return val;
}



/**
* クイックソートの実装 <br>
* @param numbers[] 配列
* @param left クイックソートする区間の左端
* @param right クイックソートする区間の右端
*
* @return acosの値
*/
void quickSort(int numbers[], int left, int right)
{
    int pivot, l_hold, r_hold;

    l_hold = left;
    r_hold = right;
    pivot = numbers[left];
    while (left < right)
    {
        while ((numbers[right] >= pivot) && (left < right))
            right--;
        if (left != right)
        {
            numbers[left] = numbers[right];
            left++;
        }
        while ((numbers[left] <= pivot) && (left < right))
            left++;
        if (left != right)
        {
            numbers[right] = numbers[left];
            right--;
        }
    }
    numbers[left] = pivot;
    pivot = left;
    left = l_hold;
    right = r_hold;
    if (left < pivot)
        quickSort(numbers, left, pivot-1);
    if (right > pivot)
        quickSort(numbers, pivot+1, right);
}


/******************************************************************************
**                            End Of File
******************************************************************************/
