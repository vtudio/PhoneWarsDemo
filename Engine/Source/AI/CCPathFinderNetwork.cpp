/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : AIPathNodeNetwork.h
 *-----------------------------------------------------------
 */


#include "CCDefines.h"


CCPathFinderNetwork::CCPathFinderNetwork()
{
}


CCPathFinderNetwork::~CCPathFinderNetwork()
{
	nodes.deleteObjectsAndList();
}


void CCPathFinderNetwork::view()
{	
    if( nodes.length > 0 )
    {
        const CCColour nodeColour = CCColour( 1.0f, 0.0f, 0.0f, 1.0f );
        const CCColour pathColour = CCColour( 0.0f, 0.0f, 1.0f, 1.0f );
        static CCVector3 start, end;
        {
            CCSetColour( nodeColour );
            
            GLVertexPointer( 3, GL_FLOAT, sizeof( PathNode ), &nodes.list[0]->point );
            glDrawArrays( GL_POINTS, 0, nodes.length );
            
            for( int i=0; i<nodes.length; ++i )
            {
                PathNode *node = nodes.list[i];
                for( int j=0; j<nodes.list[i]->numberOfConnections; ++j )
                {
                    start.set( node->point.x, 2.0f, node->point.z );
                    end.set( node->connections[j].node->point.x, 2.0f, node->connections[j].node->point.z );
                    CCRenderLine( start, end );
                }
            }
        }
        
        GLDisableDepth();
        {
            CCSetColour( pathColour );
            
            const PathNode *currentNode = pathingFrom;
            for( int i=0; i<path.endDirection; ++i )
            {
                const int connectionIndex = path.directions[i];
                if( connectionIndex < currentNode->numberOfConnections )
                {
                    const PathNode *toNode = currentNode->connections[connectionIndex].node;
                    ASSERT( toNode != NULL );
                    start.set( currentNode->point.x, 3.0f, currentNode->point.z );
                    end.set( toNode->point.x, 3.0f, toNode->point.z );
                    CCRenderLine( start, end );
                    currentNode = toNode;
                }
            }
        }
        GLEnableDepth();
    }
}


void CCPathFinderNetwork::addNode(const CCVector3 point)
{
	PathNode *node = new PathNode();
	node->point = point;
	nodes.add( node );
}


void CCPathFinderNetwork::addCollideable(CCObjectCollideable *collideable, const CCVector3 &extents)
{
	const float maxIncrement = 50.0f;
	const float minIncrement = 25.0f;
	
	const float startX = collideable->min.x - minIncrement;
	const float endX = collideable->max.x + minIncrement;
	const float width = endX - startX;
	float numberOfIncrements = roundf( ( width / maxIncrement ) + 0.5f );
	const float spacingX = width / numberOfIncrements;
	
	const float startZ = collideable->min.z - minIncrement;
	const float endZ = collideable->max.z + minIncrement;
	const float depth = endZ - startZ;
	numberOfIncrements = roundf( ( depth / maxIncrement ) + 0.5f  );
	const float spacingZ = depth / numberOfIncrements;
	
	// Front and back rows
	for( float x=startX; x<endX+1.0f; x+=spacingX )
	{
        if( x > -extents.x && x < extents.x )
        {
            for( float z=startZ; z<endZ+1.0f; z+=depth )
            {
                if( z > -extents.z && z < extents.z )
                {
                    addNode( CCVector3( x, 0.0f, z ) );
                }
            }
		}
	}
	
	// Left and right rows
	for( float z=startZ+spacingZ; z<endZ+1.0f-spacingZ; z+=depth-spacingZ )
	{
        if( z > -extents.z && z < extents.z )
        {
            for( float x=startX; x<endX+1.0f; x+=spacingX )
            {
                if( x > -extents.x && x < extents.x )
                {
                    addNode( CCVector3( x, 0.0f, z ) );
                }
            }
		}
	}
}


void CCPathFinderNetwork::addFillerNodes(CCObjectCollideable *collideable)
{
	CCUpdateCollisions( collideable );
	
	const float maxIncrement = 50.0f;
	const float minIncrement = 5.0f;
	
	const float startX = collideable->min.x + minIncrement;
	const float endX = collideable->max.x - minIncrement;
	const float width = endX - startX;
	float numberOfIncrements = roundf( ( width / maxIncrement ) + 0.5f  );
    if( numberOfIncrements < 1.0f )
    {
        numberOfIncrements = 1.0f;
    }
	const float spacingX = width / numberOfIncrements;
	
	const float startZ = collideable->min.z + minIncrement;
	const float endZ = collideable->max.z - minIncrement;
	const float depth = endZ - startZ;
	numberOfIncrements = roundf( ( depth / maxIncrement ) + 0.5f  );
    if( numberOfIncrements < 1.0f )
    {
        numberOfIncrements = 1.0f;
    }
	const float spacingZ = depth / numberOfIncrements;
	
	for( float x=startX; x<endX+1.0f; x+=spacingX )
	{
		for( float z=startZ; z<endZ+1.0f; z+=spacingZ )
		{
			addNode( CCVector3( x, 0.0f, z ) );
		}
	}
}


void CCPathFinderNetwork::clear()
{
	nodes.deleteObjects();
    pathingFrom = NULL;
}


void CCPathFinderNetwork::connect(CCObjectCollideable *objectToPath)
{
	CCVector3 savedCollisionBounds = objectToPath->collisionBounds;
	objectToPath->collisionBounds.x *= 2.0f;
	objectToPath->collisionBounds.z *= 2.0f;
	
	const float maxNodeDistance = CC_SQUARE( 70.0f );
	for( int i=0; i<nodes.length; ++i )
	{
		PathNode *currentNode = nodes.list[i];
		
		for( int j=0; j<nodes.length; ++j )
		{
			const PathNode *targetNode = nodes.list[j];
			
			if( currentNode != targetNode )
			{
				float distance = CCVector3DistanceCheck2D( currentNode->point, targetNode->point );
				if( distance < maxNodeDistance )
				{
					int connectionIndex;
					for( connectionIndex=0; connectionIndex<currentNode->numberOfConnections; ++connectionIndex )
					{
						if( distance < currentNode->connections[connectionIndex].distance )
						{
							break;
						}
					}
					
					if( currentNode->numberOfConnections < PathNode::max_connections || connectionIndex < currentNode->numberOfConnections )
					{
						// Scan to see if we're blocked by a collision
						//CollideableBase *hitObject = BasicLineCollisionCheck( currentNode->point,
						//													  targetNode->point, 
						//													  objectToPath->collisionBounds.x );
						
						CCObjectCollideable *collidedWith = CCMovementOctreeCollisionCheck( objectToPath, currentNode->point, targetNode->point );
						
						if( collidedWith == NULL )
						{
							float angle = CCAngleTowards( currentNode->point, targetNode->point );
							
							// Check to see if we already have this angle
							//if( false )
							{
								int angleFound = -1;
								for( int k=0; k<currentNode->numberOfConnections; ++k )
								{
									if( angle == currentNode->connections[k].angle )
									{
										angleFound = k;
										break;
									}
								}
								
								// We already have an angle connected closer
								if( angleFound != -1 )
								{
									if( angleFound < connectionIndex )
									{
										continue;
									}
									else
									{
										currentNode->numberOfConnections--;
										for( int k=angleFound; k<currentNode->numberOfConnections; ++k )
										{
											currentNode->connections[k] = currentNode->connections[k+1];
										}
									}
								}
							}
							
							// Insert our node
							if( connectionIndex < currentNode->numberOfConnections )
							{	
								// Continue with ordering our nodes
								for( int k=PathNode::max_connections-1; k>connectionIndex; --k )
								{
									currentNode->connections[k] = currentNode->connections[k-1];
								}
							}
							
							currentNode->connections[connectionIndex].distance = distance;
							currentNode->connections[connectionIndex].angle = angle;
							currentNode->connections[connectionIndex].node = targetNode;
							
							if( currentNode->numberOfConnections < PathNode::max_connections )
							{
								currentNode->numberOfConnections++;
							}
							else
							{
								currentNode->numberOfConnections = currentNode->numberOfConnections;
							}
						}

					}
					else
					{
						currentNode->numberOfConnections = currentNode->numberOfConnections;
					}
				}
			}
		}
	}
	
	objectToPath->collisionBounds = savedCollisionBounds;
}


uint CCPathFinderNetwork::findClosestNodes(const CCVector3 &position, const float &radius, const CCVector3 **vectors, const uint &length)
{
	uint found = 0;
	
	for( int i=0; i<nodes.length && found<length; ++i )
	{
		const PathNode *node = nodes.list[i];
		if( node->numberOfConnections > 0 )
		{
			float distance = CCVector3DistanceCheck2D( position, node->point );
			if( distance < radius )
			{
				vectors[found++] = &node->point;
			}
		}
	}
	
	return found;
}
	

const CCPathFinderNetwork::PathNode* CCPathFinderNetwork::findClosestNode(CCObjectCollideable *objectToPath, const CCVector3 *position, const bool withConnections)
{
	int closestNode = -1;
	float closestDistance = MAXFLOAT;
	
	for( int i=0; i<nodes.length; ++i )
	{
		PathNode *node = nodes.list[i];
		if( withConnections == false || node->numberOfConnections > 0 )
		{
			const float distance = CCVector3DistanceCheck2D( node->point, *position );
			if( distance < closestDistance )
			{
                CCObjectCollideable *hitObject = CCMovementOctreeCollisionCheck( objectToPath, *position, node->point );
                if( hitObject != NULL )
                {
                    continue;
                }
                if( hitObject == NULL )
                {
                    closestDistance = distance;
                    closestNode = i;
                }
			}
		}
	}
	
	if( closestNode != -1 )
	{
		return nodes.list[closestNode];
	}
	
	return NULL;
}

static const CCPathFinderNetwork::PathNode *compareNode;
static const CCPathFinderNetwork::PathNode *targetNode;
static int compare(const void *a, const void *b)
{
	const CCPathFinderNetwork::PathNode::PathConnection &pathA = compareNode->connections[ *(int*)a ];
	const CCPathFinderNetwork::PathNode::PathConnection &pathB = compareNode->connections[ *(int*)b ];
	
	const float pathADistance = CCVector3DistanceCheck2D( pathA.node->point, targetNode->point );
	const float pathBDistance = CCVector3DistanceCheck2D( pathB.node->point, targetNode->point );
	
	return ( pathADistance - pathBDistance );
}


void CCPathFinderNetwork::findPath(Path &pathResult, const PathNode *fromNode, const PathNode *toNode)
{	
    if( fromNode != NULL && toNode != NULL )
    {
        NodesList<const PathNode, 50> previousNodes;
        previousNodes.add( fromNode );
        
        targetNode = toNode;
        path.distance = 0.0f;
        followPath( path, 0, 0.0f, previousNodes, fromNode, toNode );
        
        pathingFrom = fromNode;
        pathResult = path;
    }
}


bool CCPathFinderNetwork::followPath(Path &path, 
                                     const int currentDirection, 
                                     const float currentDistance, 
                                     NodesList<const PathNode, 50> &previousNodes, 
                                     const PathNode *fromNode, const PathNode *toNode)
{	
	// Node found
	if( fromNode == toNode )
	{
		path.endDirection = currentDirection;
		path.distance = currentDistance;
		return true;
	}
	
	const int nextDirection = currentDirection+1;
	if( nextDirection >= 50 )
	{
		// Give up
		return false;
	}
	
	compareNode = fromNode;
	int values[PathNode::max_connections];
	for( int i=0; i<fromNode->numberOfConnections; ++i )
	{
		values[i] = i;
	}
	qsort( values, fromNode->numberOfConnections, sizeof( int ), compare );
	
	for( int i=0; i<fromNode->numberOfConnections; ++i )
	{
		const PathNode::PathConnection *nextConnection = &fromNode->connections[values[i]];
		
		// Previously followed?
		if( previousNodes.find( nextConnection->node ) != -1 )
		{
			continue;
		}

		const float pathDistance = currentDistance + nextConnection->distance;
		if( previousNodes.add( nextConnection->node ) == false )
		{
			return false;
		}
		
		if( followPath( path, nextDirection, pathDistance, previousNodes, nextConnection->node, toNode ) )
		{
			path.directions[currentDirection] = values[i];
			return true;
		}
	}
	
	return false;
}
