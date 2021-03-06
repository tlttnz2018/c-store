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
 *  MResultNode.h
 *  cstore
 *
 *  Created by Nga Tran on 3/31/05.
 *  
 *	Output results
 */

#ifndef _MCONCATNODE_H_
#define _MCONCATNODE_H_

#include <string>
#include <list>
#include "../MNode.h"
#include "../PlanConstants.h"
#include "../../Operators/BlockPrinter.h"

using namespace std;

class MResultNode : public MNode
{
	protected:
		bool m_bIsAggreggate;
		bool m_bhasGroupby;
		bool m_bDoMerge;
		
	protected:
		// Node's constructor is protected because they should not be new directly.
		// A node can be new by using static "create" function
		// This is for the purpose of memory management
		MResultNode();
		MResultNode(list<Node*>	lpChildren);
		
	public:
		virtual ~MResultNode();
		
		// set if merge should be done
		virtual void setDoMerge(bool bDoMerge);
		
		
		// There is aggragte in the results
		virtual void setAggregate();
		
		// There is group by
		virtual void setGroupby();
		
		// Merge ROS and WOS operators
		virtual Operator* mergeROSandWOS();
		
		// Run the operator of this node
		virtual Operator* run(bool rosOnly = 1);
		
		// Run ROS
		virtual Operator* runROS();
		
		// Run WOS
		virtual Operator* runWOS();
		
		// show what operator this node will run
		virtual void show(int* lpNodeID, ofstream* lpOfstream, bool rosOnly = 1);
		
		// show tree presentation
		virtual void showTree(string sTabs);
		
		// show merge operators
		virtual void showMerge(ofstream* lpOfstream);
		
	//---------------------------------------------------------------
	// Memory management functions
		
	public:		
		// New a node and put it in the m_lpList
		static MResultNode* create();
		static MResultNode* create(list<Node*>	lpChildren);
};

#endif // _MCONCATNODE_H_




