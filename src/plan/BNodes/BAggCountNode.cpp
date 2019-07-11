/* Copyright (c) 2005, Regents of Massachusetts Institute of Technology, 
 * Brandeis University, Brown University, and University of Massachusetts 
 * Boston. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *   - Neither the name of Massachusetts Institute of Technology, 
 *     Brandeis University, Brown University, or University of 
 *     Massachusetts Boston nor the names of its contributors may be used 
 *     to endorse or promote products derived from this software without 
 *     specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  BAggCountNode.cpp
 *  cstore
 *
 *  Created by Nga Tran on 3/30/05.
 *  
 *	Count node  with group by
 */

#include "BAggCountNode.h"

#include <iostream>


// Default
BAggCountNode::BAggCountNode() : BAggNode()
{
	m_lpMergeOp = NULL;
}


// Initialize with column to be aggregated
BAggCountNode::BAggCountNode(Node* lpAggCol, int iAggColIndex, Node* lpAggGroup, int iGroupColIndex) : BAggNode(lpAggCol, iAggColIndex, lpAggGroup, iGroupColIndex)
{
	m_lpMergeOp = NULL;
}


// Deallocate memory
BAggCountNode::~BAggCountNode()
{
	 Log::writeToLog("BAggCountNode", 10, "~BAggCountNode");
	 
	 if (m_lpROSReturnOp != NULL)
	 {
		delete m_lpROSReturnOp;
		m_lpROSReturnOp = NULL;
	 }
	 
	 if (m_lpWOSReturnOp != NULL)
	 {
		delete m_lpWOSReturnOp;
		m_lpWOSReturnOp = NULL;
	 } 
	 
	 if (m_lpReturnOp != NULL)
	 {
		delete m_lpReturnOp;
		m_lpReturnOp = NULL;
	 } 
	 
	 if (m_lpROSIdHashFunction != NULL)
	 {
		delete m_lpROSIdHashFunction;
		m_lpROSIdHashFunction = NULL;
	 }
	 
	 if (m_lpWOSIdHashFunction != NULL)
	 {
		delete m_lpWOSIdHashFunction;
		m_lpWOSIdHashFunction = NULL;
	 }
	
}

// Merge ROS and WOS operators
Operator* BAggCountNode::mergeROSandWOS()
{
	if (m_lpReturnOp != NULL)
	{
		// Has been merged
		return m_lpReturnOp;
	}
	
	if ( (m_lpROSReturnOp != NULL) && (m_lpWOSReturnOp != NULL) )
	{
		// Merge them
		m_lpReturnOp = new MergeSortedGroups(	m_lpROSReturnOp,
												m_lpWOSReturnOp,
                                                2,      // num columns
												0);     // merge by col index
		
		((MergeSortedGroups*) m_lpReturnOp)->setMergeOp(new MergeCountOp());
		
		return m_lpReturnOp;
	}
	
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}
	
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}
	
	return m_lpReturnOp;
}


// Run ROS only
Operator* BAggCountNode::runROS()
{
	// The operator exists, only return it
	if (m_lpROSReturnOp != NULL)
	{
		return m_lpROSReturnOp;
	}
	
	// Left is group, right is aggregate
	Operator* lpLeft = NULL;
	Operator* lpRight = NULL;
	
	if (m_lpLeft != NULL)
	{
		lpLeft = m_lpLeft->runROS();
	}
	
	if (m_lpRight != NULL)
	{
		lpRight = m_lpRight->runROS();
	}
		
	if ( (lpLeft != NULL) && (lpRight != NULL) )
	{  
		if (m_bUnsortedGroupBy)
		{
			// Unsorted groupy column, use hash aggregate
			m_lpROSReturnOp = new HashCount(lpRight, m_iAggColIndex, lpLeft, m_iGroupColIndex);
			m_lpROSIdHashFunction = new IdentityHashFunction(m_iHashTableSize);
			((HashAggregator*) m_lpROSReturnOp)->setHashFunction(m_lpROSIdHashFunction);
			((HashAggregator*) m_lpROSReturnOp)->setHashTableSize(m_iHashTableSize);
		}
		else
		{
			m_lpROSReturnOp = new Count(lpRight, m_iAggColIndex, lpLeft, m_iGroupColIndex);
		}
	}
		
	return m_lpROSReturnOp;
}

// Run WOS only
Operator* BAggCountNode::runWOS()
{
	// The operator exists, only return it
	if (m_lpWOSReturnOp != NULL)
	{
		return m_lpWOSReturnOp;
	}
	
	// Left is group, right is aggregate
	Operator* lpLeft = NULL;
	Operator* lpRight = NULL;
	
	if (m_lpLeft != NULL)
	{
		lpLeft = m_lpLeft->runWOS();
	}
	
	if (m_lpRight != NULL)
	{
		lpRight = m_lpRight->runWOS();
	}
		
	if ( (lpLeft != NULL) && (lpRight != NULL) )
	{
		if (m_bUnsortedGroupBy)
		{
			// Unsorted groupy column, use hash aggregate
			m_lpWOSReturnOp = new HashCount(lpRight, m_iAggColIndex, lpLeft, m_iGroupColIndex);
			m_lpWOSIdHashFunction = new IdentityHashFunction(m_iHashTableSize);
			((HashAggregator*) m_lpWOSReturnOp)->setHashFunction(m_lpWOSIdHashFunction);
			((HashAggregator*) m_lpWOSReturnOp)->setHashTableSize(m_iHashTableSize);
		}
		else
		{
			m_lpWOSReturnOp = new Count(lpRight, m_iAggColIndex, lpLeft, m_iGroupColIndex);
		}
	}
		
	return m_lpWOSReturnOp;
}

// show what operator this node will run
void BAggCountNode::showROS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bROSHasShown)
	{
		return;
	}
	
	commonShowROS(lpNodeID, "Count", lpOfstream);
}

// show what operator this node will run
void BAggCountNode::showWOS(int* lpNodeID, ofstream* lpOfstream)
{
	// The operator exists
	if (m_bWOSHasShown)
	{
		return;
	}
	
	commonShowWOS(lpNodeID, "Count", lpOfstream);
}

// show merge operators
void BAggCountNode::showMerge(ofstream* lpOfstream)
{
	commonShowMerge("Count", lpOfstream);
}

// show tree presentation
void BAggCountNode::showTree(string sTabs)
{
	// The operator exists
	if (m_bTreeHasShown)
	{
		cout << endl;
		cout << sTabs << "COUNT AGGREGATE - The operator was created before" << endl;
		
		return;
	}
	
	m_bTreeHasShown = 1;
	
	// Show its information first
	cout << endl;
	cout << sTabs << "COUNT AGGREGATE" << endl;
	
	commonShowTree("Count", sTabs);
	sTabs = sTabs + "	";
		
	// Show child's information
	if (m_lpLeft != NULL)
	{
		m_lpLeft->showTree(sTabs);
	}
	
	if (m_lpRight != NULL)
	{
		m_lpRight->showTree(sTabs);
	}
}


//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
BAggCountNode* BAggCountNode::create()
{
	Log::writeToLog("BAggCountNode", 10, "Create without arguments");
		
	BAggCountNode* lpNode = new BAggCountNode();
	pushBack(lpNode);
	return lpNode;
}

// New a node and put it in the m_lpList
BAggCountNode* BAggCountNode::create(Node* lpAggCol, int iAggColIndex, Node* lpAggGroup, int iGroupColIndex)
{
	Log::writeToLog("BAggCountNode", 10, "Create with arguments: Node* lpAggCol, int iAggColIndex, Node* lpAggGroup, int iGroupColIndex");
		
	BAggCountNode* lpNode = new BAggCountNode(lpAggCol, iAggColIndex, lpAggGroup, iGroupColIndex);
	pushBack(lpNode);
	return lpNode;
}
