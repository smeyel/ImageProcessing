#ifndef __SEQUENCECOUNTERTREENODE_H
#define __SEQUENCECOUNTERTREENODE_H

#include <opencv2/opencv.hpp>			// For OPENCV_ASSERT
//#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)

#include "LutColorFilter.h"

using namespace std;

#define MAXNODECOUNTERNUM	5

namespace smeyel
{
	/** Represents a node of a tree (or graph) which can make statistics from the possible transitions
		in an input sequence.
	*/
	class SequenceCounterTreeNode
	{
		/** Next free nodeID. Used by the constructor to assign unique ID-s. */
		static int nextFreeNodeID;

		/** Unqique ID of the node. Assigned by the constructor. */
		int nodeID;
		/** Array of child node pointers. */
		SequenceCounterTreeNode **children;

		/** Number of children. Set by the constructor and should not be changed later. */
		int inputValueNumber;

		/** Counter values which can be used for any purpose. */
		int counter[MAXNODECOUNTERNUM];

		/** Helper function to present indented tree representations. */
		void writeIndent(int indent);
	public:
		/** Auxiliary number, can be used freely for any purpose. */
		unsigned char auxScore;
		/** Auxiliary number, can be used freely for any purpose. */
		unsigned int status;

		/** Constructor
			@param inputValueNumber	The number of possible input values (and thus, child nodes)
		*/
		SequenceCounterTreeNode(const int inputValueNumber);

		/** Destructor */
		~SequenceCounterTreeNode();

		/** Returns the child node for given input value.
			@param inputValue	The input for which the child node is returned.
			@param createIfNotPresent	If true, non-existing child nodes are created (on demand creating).
			@return	The child node asked for, or NULL if it does not exist and createIfNotPresent==false.
		*/
		SequenceCounterTreeNode *getChildNode(const unsigned int inputValue, bool createIfNotPresent=false);

		/** Returns the input value for which the given child is assigned. */
		int getInputValueForChild(SequenceCounterTreeNode *child);

		/** Returns the node for a given input sequence.
			@param inputValues	pointer to an array of input values
			@param numberOfValues	the number of input values the function should use from the array given by inputValues.
			@param createIfNotExisting	If true, nodes are created on demand.
			@return	The node corresponding to the input sequence or NULL, if it does not exists and createIfNotExisting is false.
		*/
		SequenceCounterTreeNode *getNode(const unsigned int *inputValues, const int numberOfValues, bool createIfNotExisting);

		/** Increments the values of the internal counter.
			@param counterIdx Index of the counter to increment. Must be < MAXNODECOUNTERNUM. */
		void incrementCounter(int counterIdx);

		/** Returns the value of the given counter. */
		int getCounter(int counterIdx);

		/** Returns the sum of the given counter in the subtree of the node (including the current node as root.) */
		int getSubtreeSumCounter(int counterIdx);

		/** Overwrites counter of current node with sum of children, except if there are no children (or sum is 0). */
		int calculateSubtreeCounters(int counterIdx);

		/** Shows the subtree of the node recursively on cout. */
		void showRecursive(int indent, int maxCounterIdx, bool showNullChildren);

		/** Similar to showRecursive, but the representation is more compact. */
		void showCompactRecursive(int indent, int maxCounterIdx, vector<string> *inputValueNames=NULL);

		/** Returns the ID of the node. */
		int getNodeID()
		{
			return nodeID;
		}

		/** Returns inputValueNumber. */
		int getInputValueNumber()
		{
			return inputValueNumber;
		}

		/** Clears all child node pointers.
			@warning The destructor deletes all child nodes, so if that is not the desired behaviour, call this before delete!
		*/
		void disconnectAllChildren()
		{
			for(int i=0; i<inputValueNumber; i++)
			{
				children[i]=NULL;
			}
		}

		/** Decides whether this node is a parent node of the given node. */
		bool isParentOf(SequenceCounterTreeNode *node);

		// --------- graph modification functions
	private:
		/** Replaces oldNode with newNode among the child pointers.
			Used by redirectParents(). 
		*/
		void replaceChildPointer(SequenceCounterTreeNode *oldNode, SequenceCounterTreeNode *newNode);
		
		/** Redirects all pointers pointing to oldNode with newNode. Processes the subtree of startNode.
			Used by combineNodes
		*/
		static void redirectParents(SequenceCounterTreeNode *startNode,SequenceCounterTreeNode *oldNode,SequenceCounterTreeNode *newNode);
		/** Asks for a node representing the merge of nodeA and nodeB. If one of them is NULL, the other one will be returned.
			If both exist, a new node is created.
			@warning	Replaced nodes are not deleted!
			Used by combineNodes.
		*/
		static SequenceCounterTreeNode *getMergedNode(SequenceCounterTreeNode *root, SequenceCounterTreeNode *nodeA,SequenceCounterTreeNode *nodeB);
	public:
		/** Combines two nodes into one and updates the graph accordingly.
			@warning Nodes not used anymore are not removed!
		*/
		static void combineNodes(SequenceCounterTreeNode *root, SequenceCounterTreeNode *nodeA, SequenceCounterTreeNode *nodeB);

	public:
		/** Cuts subtree if the status of all nodes in it is the same as the status of this one (status).
			Otherwise, tries with the child nodes.
			@param parentStatus	The status property of the parent node.
				It only influences the return value.
			@return true if the node can be removed as all children could be removed and its status is the same as the parents.
			@warning: use only on real trees, not suitable for DAG-s!
		*/
		bool cut(unsigned int parentStatus = 0);
	};
}

#endif
