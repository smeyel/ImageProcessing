#include "PixelPrecisionCalculator.h"


using namespace smeyel;

float PixelPrecisionCalculator::getNodePrecision(SequenceCounterTreeNode *node)
{
	float onNum = (float)node->getCounter(counterIdxOn);
	float offNum = (float)node->getCounter(counterIdxOff);
	float precision = onNum / (onNum+offNum);
	return precision;
}

void PixelPrecisionCalculator::setPrecisionStatus(SequenceCounterTreeNode *node, float minPrecision)
{
	if (node==NULL)
	{
		return;
	}

	float precision = getNodePrecision(node);
	if (precision >= minPrecision)
	{
		node->status = STATUS_HIGHPRECISION;
	}
	else
	{
		node->status = STATUS_LOWPRECISION;
	}
	node->auxScore = (unsigned char)(128.0F + (precision-minPrecision)/(1.0F-minPrecision)*127.0F);

	int n = node->getInputValueNumber();
	// first, recursive call
	for(int i=0; i<n; i++)
	{
		setPrecisionStatus(node->getChildNode(i), minPrecision);
	}
}
