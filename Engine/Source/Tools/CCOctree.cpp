/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCOctree.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCObjects.h"
#include "CCCollisionTools.h"
#include "CCSceneBase.h"


#ifdef DEBUGON
int maxOctreesPerObject = 0;
int maxLeafsPerScan = 0;
int maxCollideablesPerScan = 0;
#endif


CCOctree::CCOctree(CCOctree *inParent, const CCVector3 position, const float size)
{
	parent = inParent;
	leafs = NULL;
	objects.allocate( MAX_TREE_OBJECTS );
	
	hSize = size * 0.5f;
	min = position;
	max = position;
	min.add( -hSize );
	max.add( hSize );
}


void CCOctreeDeleteLeafs(CCOctree *tree)
{
	// Ensure all our leafs are deleted
	if( tree->leafs != NULL )
	{
		for( uint i=0; i<8; ++i )
		{
			if( tree->leafs[i] != NULL )
			{
				CCOctree *leaf = tree->leafs[i];
				CCOctreeDeleteLeafs( leaf );
			}
		}
		
		delete tree->leafs;
		tree->leafs = NULL;
	}
}


// Create the top leafs
void CCOctreeSplitTopLeafs(CCOctree *tree, const uint index, CCVector3 position)
{
	position.y += tree->hSize;
	CCOctree **leaf = &( tree->leafs[index+4] );
	*leaf = new CCOctree( tree, position, tree->hSize );
}


void CCOctreeSplit(CCOctree *tree)
{
	ASSERT( tree->leafs == NULL );
	tree->leafs = (CCOctree**)malloc( sizeof( CCOctree* ) * 8 );
	
	// Create our leaf nodes
	uint index = leaf_bottom_front_left;
	CCVector3 position = tree->min;
    position.add( tree->hSize * 0.5f );
	CCOctree **leaf = &( tree->leafs[index] );
	*leaf = new CCOctree( tree, position, tree->hSize );
	CCOctreeSplitTopLeafs( tree, index, position );
	
	index = leaf_bottom_front_right;
	position.x += tree->hSize;
	leaf = &( tree->leafs[index] );
	*leaf = new CCOctree( tree, position, tree->hSize );
	CCOctreeSplitTopLeafs( tree, index, position );
	
	index = leaf_bottom_back_right;
	position.z += tree->hSize;
	leaf = &( tree->leafs[index] );
	*leaf = new CCOctree( tree, position, tree->hSize );
	CCOctreeSplitTopLeafs( tree, index, position );
	
	index = leaf_bottom_back_left;
	position.x -= tree->hSize;
	leaf = &( tree->leafs[index] );
	*leaf = new CCOctree( tree, position, tree->hSize );
	CCOctreeSplitTopLeafs( tree, index, position );
	
	// Now we need to sort our objects into our new leafs
	while( tree->objects.length > 0 )
	{
		CCObjectCollideable *collideable = tree->objects.list[0];
        CCOctreeRemoveObject( tree, collideable );
		
		for( uint i=0; i<8; ++i )
		{
			CCOctree *leaf = tree->leafs[i];
			if( CCOctreeIsInLeaf( leaf, collideable->min, collideable->max ) )
			{
				CCOctreeAddObject( leaf, collideable );
			}
		}
	}
}


void CCOctreeAddObject(CCOctree *tree, CCObjectCollideable *collideable)
{ 
	// Ensure the object is within the octree's limits
	if( tree->parent == NULL )
	{
		CCUpdateCollisions( collideable, false );
		CCVector3 *objectMin = &collideable->min;
		CCVector3 *objectMax = &collideable->max;
		if( objectMax->x < tree->min.x )
			return;
		
		if( objectMax->y < tree->min.y )
			return;
		
		if( objectMax->z < tree->min.z )
			return;
		
		if( objectMin->x > tree->max.x )
			return;
		
		if( objectMin->y > tree->max.y )
			return;
		
		if( objectMin->z > tree->max.z )
			return;
	}
	
	// Insert in an approperate leaf node
	if( tree->leafs != NULL )
	{
        CCOctreeRemoveObject( tree, collideable );
		
		for( uint i=0; i<8; ++i )
		{
			CCOctree *leaf = tree->leafs[i];
			if( CCOctreeIsInLeaf( leaf, collideable->min, collideable->max ) )
			{
				CCOctreeAddObject( leaf, collideable );
			}
		}
	}
	
	// Unless we don't have any
    else if( tree->objects.length < MAX_TREE_OBJECTS )
	{
        tree->objects.add( collideable );
		collideable->octrees.add( tree );
        LOG_NEWMAX( "Max trees per object", maxOctreesPerObject, collideable->octrees.length );
	}

    // Give up because the tree is too small..
    else if( tree->hSize < 100.0f )
    {
        tree->objects.add( collideable );
        collideable->octrees.add( tree );
        LOG_NEWMAX( "Max trees per object", maxOctreesPerObject, collideable->octrees.length );
    }
	
	// If we have too many objects split the octree
	else
	{
		CCOctreeSplit( tree );
		
		// Try again
		CCOctreeAddObject( tree, collideable );
	}
}
                           
                           
void CCOctreeRemoveObject(CCOctree *tree, CCObjectCollideable *collideable)
{
    tree->objects.remove( collideable );
    collideable->octrees.remove( tree );
}

		
void CCOctreeRemoveObject(CCObjectCollideable *collideable)
{
	while( collideable->octrees.length > 0 )
	{	
		CCOctree *tree = collideable->octrees.list[0];
        CCOctreeRemoveObject( tree, collideable );
		
		if( tree->objects.length == 0 )
		{
			if( gEngine->collisionManager.pruneTreesTimer <= 0.0f )
			{
				gEngine->collisionManager.pruneTreesTimer = 0.0f;
			}
		}
	}
}


void CCOctreeRefreshObject(CCObjectCollideable *collideable)
{
	CCOctreeRemoveObject( collideable );
	if( collideable->isActive() )
	{
		CCOctreeAddObject( gEngine->collisionManager.tree, collideable );
	}
	
	//ASSERT( collideable->numberOfOctrees > 0 );
}


void CCOctreePruneTree(CCOctree *tree)
{
	if( tree->leafs != NULL )
	{
		bool hasObjects = CCOctreeHasObjects( tree );
		if( hasObjects == false )
		{
			CCOctreeDeleteLeafs( tree );
		}
		else
		{
			for( uint i=0; i<8; ++i )
			{
				CCOctreePruneTree( tree->leafs[i] );
			}
		}
	}
}


bool CCOctreeIsInLeaf(const CCOctree *leaf, const CCVector3 &targetMin, const CCVector3 &targetMax)
{
	const CCVector3 *sourceMin = &leaf->min;
	const CCVector3 *sourceMax = &leaf->max;
	
	if( sourceMax->y >= targetMin.y && sourceMin->y <= targetMax.y )
	{
		if( sourceMax->x >= targetMin.x && sourceMin->x <= targetMax.x )
		{
			if( sourceMax->z >= targetMin.z && sourceMin->z <= targetMax.z )
			{
				return true;
			}
		}
	}
	
	return false;
}


bool CCOctreeHasObjects(CCOctree *tree)
{
	bool hasObjects = tree->objects.length > 0;
	
	if( tree->leafs != NULL )
	{
		for( uint i=0; i<8 && hasObjects == false; ++i )
		{
			hasObjects |= CCOctreeHasObjects( tree->leafs[i] );
		}
	}
	
	return hasObjects;
}


void CCOctreeRender(CCOctree *tree)
{
	if( tree->parent == NULL )
	{
		//GLDisableDepth();
	}
		
	if( tree->objects.length > 0 )
	{
		const float full = tree->objects.length / (float)MAX_TREE_OBJECTS;
		//glLineWidth( tree->numberOfObjects * 0.05f );
        CCColour colour( full, 1.0f - full, 0.0f, 1.0f );
		CCSetColour( colour );
		CCRenderCubeMinMax( tree->min, tree->max, true );
	}
	
	if( tree->leafs != NULL )
	{
		for( uint i=0; i<8; ++i )
		{
			CCOctreeRender( tree->leafs[i] );
		}
	}
	
	if( tree->parent == NULL )
	{
		//GLEnableDepth();
		//glLineWidth( LINE_WIDTH );
	}
}


void CCOctreeListCollideables(CCObjectCollideable **collideables, int *numberOfCollideables, const CCOctree **leafs, const int numberOfLeafs)
{
	for( int leafIndex=0; leafIndex<numberOfLeafs; ++leafIndex )
	{
		const CCOctree *leaf = leafs[leafIndex];
		for( int i=0; i<leaf->objects.length; ++i )
		{
			CCObjectCollideable *collideable = leaf->objects.list[i];
			
			if( collideable->isActive() )
			{
				if( CCHasFlag( collideable->collideableType, collision_none ) == false )
				{
					bool found = false;
					for( int collideablesIndex=0; collideablesIndex<*numberOfCollideables; ++collideablesIndex )
					{
						if( collideables[collideablesIndex] == collideable )
						{
							found = true;
							break;
						}
					}
					
					if( found == false )
					{
						collideables[(*numberOfCollideables)++] = collideable;
					}
				}
			}
		}
	}
}


void CCOctreeListVisibles(CCList<CCOctree> &leafs, CCList<CCObjectCollideable> &collideables)
{
	for( int leafIndex=0; leafIndex<leafs.length; ++leafIndex )
	{
		const CCOctree *leaf = leafs.list[leafIndex];
		for( int i=0; i<leaf->objects.length; ++i )
		{
			CCObjectCollideable *collideable = leaf->objects.list[i];
			
			if( collideable->isActive() )
			{
                if( collideable->shouldRender && collideable->octreeRender )
				{
					bool found = false;
					for( int collideablesIndex=0; collideablesIndex<collideables.length; ++collideablesIndex )
					{
						if( collideables.list[collideablesIndex] == collideable )
						{
							found = true;
							break;
						}
					}
					
					if( found == false )
					{
						collideables.list[collideables.length++] = collideable;
					}
				}
			}
		}
	}
}


void CCOctreeListLeafs(const CCOctree *tree, const CCVector3 &targetMin, const CCVector3 &targetMax, const CCOctree **leafsList, int *numberOfLeafs)
{	
	if( tree->leafs != NULL )
	{
		for( uint i=0; i<8; ++i )
		{
            CCOctreeListLeafs( tree->leafs[i], targetMin, targetMax, leafsList, numberOfLeafs );
		}
	}
	else if( tree->objects.length > 0 )
	{
		for( int i=0; i<*numberOfLeafs; ++i )
		{
			if( leafsList[i] == tree )
			{
				return;
			}
		}
		
		if( CCOctreeIsInLeaf( tree, targetMin, targetMax ) )
		{
			leafsList[(*numberOfLeafs)++] = tree;
		}
	}
}


inline void fillLeafsInFrustum(const float frustum[6][4], CCOctree *tree, CCList<CCOctree> &leafs)
{	
    if( tree->leafs != NULL )
    {
        for( uint i=0; i<8; ++i )
        {
            fillLeafsInFrustum( frustum, tree->leafs[i], leafs );
        }
    }
    else
    {
        bool found = false;
        for( int i=0; i<leafs.length; ++i )
        {
            if( leafs.list[i] == tree )
            {
                found = true;
                break;
            }
        }

        if( found == false && CCCubeInFrustum( frustum, tree->min, tree->max ) )
        {
            leafs.list[leafs.length++] = tree;
        }
    }
}


static void collideablesInFrustum(const float frustum[6][4], 
                                  CCList<CCObjectCollideable> &octreeCollideables, 
                                  CCList<CCObjectCollideable> &visibleCollideables)
{
    for( int i=0; i<octreeCollideables.length; ++i )
    {
        CCObjectCollideable *collideable = octreeCollideables.list[i];
        if( CCCubeInFrustum( frustum, collideable->min, collideable->max ) )
        {
            collideable->visible = true;
            visibleCollideables.add( collideable );
        }
    }
}


CCObjectCollideable* CCOctreeGetVisibleCollideables(const int i)
{
	return CCCameraBase::currentCamera->visibleCollideables.list[i];
}


void CCOctreeScanVisibleCollideables(const float frustum[6][4], 
                                     CCList<CCObjectCollideable> &visibleCollideables)
{
#define MAX_LEAFS 512
    static CCList<CCOctree> leafs( MAX_LEAFS );
    static CCList<CCObjectCollideable> octreeCollideables( MAX_VISIBLE_COLLIDEABLES );

    // First find all the octrees that collide with the frustum
    leafs.length = 0;
    fillLeafsInFrustum( frustum, gEngine->collisionManager.tree, leafs );
    ASSERT( leafs.length < MAX_LEAFS );
    LOG_NEWMAX( "Max leafs per scan", maxLeafsPerScan, leafs.length );

    // Then list all the objects from the octrees
    octreeCollideables.length = 0;
    CCOctreeListVisibles( leafs, octreeCollideables );
    ASSERT( octreeCollideables.length < MAX_VISIBLE_COLLIDEABLES );

    // Finally find all the collideables that collide with the frustum
    visibleCollideables.length = 0;
    collideablesInFrustum( frustum, octreeCollideables, visibleCollideables );
    ASSERT( visibleCollideables.length < MAX_VISIBLE_COLLIDEABLES );
    LOG_NEWMAX( "Max collideables per scan", maxCollideablesPerScan, visibleCollideables.length );
}


void CCScanVisibleCollideables(const float frustum[6][4], 
                               CCList<CCObjectCollideable> &collideables,
                               CCList<CCObjectCollideable> &visibleCollideables)
{
    // Finally find all the collideables that collide with the frustum
    visibleCollideables.length = 0;
    collideablesInFrustum( frustum, collideables, visibleCollideables );
    ASSERT( visibleCollideables.length < MAX_VISIBLE_COLLIDEABLES );
    LOG_NEWMAX( "Max collideables per scan", maxCollideablesPerScan, visibleCollideables.length );
}


static int (*visibleCompareFunction)(const void *, const void *) = NULL;
void CCOctreeSetRenderSortCallback(int (*callback)(const void *, const void *) )
{
	visibleCompareFunction = callback;
};

void CCOctreeRenderVisibleObjects(CCCameraBase *camera, const int pass, const bool alpha)
{
    CCList<CCObjectCollideable> &visibleCollideables = camera->visibleCollideables;
	if( pass == render_main && alpha && visibleCompareFunction != NULL )
	{
		qsort( camera->sortedVisibleCollideables, visibleCollideables.length, sizeof( int ), visibleCompareFunction );
	}
	
	for( int i=0; i<visibleCollideables.length; ++i )
	{
		CCObjectCollideable *object = visibleCollideables.list[camera->sortedVisibleCollideables[i]];
		if( object->renderPass == pass )
		{
            if( object->inScene != NULL )
            {
                // Ask the scene if we should render this obejct
                object->inScene->renderOctreeObject( object, camera, pass, alpha );
            }
            else
            {
				DEBUGLOG( "CCOctreeRenderVisibleObjects ERROR: Object has no scene : %s\n", object->getDebugName() );
            }
		}	
	}
}
