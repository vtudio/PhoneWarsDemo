/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : AIPathNodeNetwork.h
 * Description : Network of nodes used for path finding.
 *
 * Created     : 03/05/10
 *-----------------------------------------------------------
 */

#ifndef __CCPATHFINDERNETWORK_H__
#define __CCPATHFINDERNETWORK_H__


class CCPathFinderNetwork
{
public:
	CCPathFinderNetwork();
	~CCPathFinderNetwork();
	
	void view();
	
	void addNode(const CCVector3 point);
	void addCollideable(CCObjectCollideable *collideable, const CCVector3 &extents);
	void addFillerNodes(CCObjectCollideable *collideable);
	
	void clear();
	
	// Connect our nodes
	void connect(CCObjectCollideable *objectToPath);
	
    uint findClosestNodes(const CCVector3 &position, const float &radius, const CCVector3 **vectors, const uint &length);
	
	struct PathNode
	{
		PathNode()
		{
			numberOfConnections = 0;
		}
		
		CCVector3 point;
		
		struct PathConnection
		{
			float distance;
			float angle;
			const PathNode *node;
		};
		
		enum { max_connections = 18 };
		PathConnection connections[max_connections];
		int numberOfConnections;
	};
	const PathNode* findClosestNode(CCObjectCollideable *objectToPath, const CCVector3 *position, const bool withConnections);
	
	struct Path
	{
		Path()
		{
			endDirection = 0;
			distance = 0.0f;
		}
		
		int directions[50];
		int endDirection;
		float distance;
	};
	void findPath(Path &pathResult, const PathNode *fromNode, const PathNode *toNode);
	
protected:
	template <typename T, int TLENGTH> struct NodesList : public CCList<T>
	{
		NodesList()
		{
			this->allocate( TLENGTH );
		}
		
        bool add(T *node)
		{
			CCList<T>::add( node );
			if( this->length < this->allocated )
			{
				return true;
			}
			return false;
		}
	};
	
	// Used for path finding
	Path path;
	const PathNode *pathingFrom;
    bool followPath(Path &path, const int currentDirection,
                    const float currentDistance,
                    NodesList<const PathNode, 50> &previousNode,
                    const PathNode *fromNode, const PathNode *toNode);
					
	NodesList<PathNode, 500> nodes;
};


#endif // __CCPATHFINDERNETWORK_H__
