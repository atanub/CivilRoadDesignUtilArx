#include "Stdafx.h"
#include <math.h>
#include "TIN.h"



//To be called from other external func(s)
//************************************************************************************************
int TIN::Triangulate(AcGePoint3dArray& Pt3DArray, CTrianglePtrArray& TrgArray)
{
	AcGePoint3d* pArrayOfPts;
	TRIANGLE* pTriangles;
	int i, iNoOfPts, iMaxNoOfTrgs, iNoOfTrgs;
	
	//Allocate Memory...
	pArrayOfPts = (AcGePoint3d*)malloc((Pt3DArray.length() + 10) * sizeof(AcGePoint3d));//10 Extra points...
	if(pArrayOfPts == 0L)
	{
		ads_printf("\nERROR: TRIANGULATE - unable to allocate enough memory\n");
		return 0;
	}
	iMaxNoOfTrgs = 2 * (Pt3DArray.length() + 10);
	pTriangles = (TRIANGLE*)malloc(iMaxNoOfTrgs * sizeof(TRIANGLE));
	if(pTriangles == 0L)
	{
		ads_printf("\nERROR: TRIANGULATE - unable to allocate enough memory\n");
		free(pArrayOfPts);
		return 0;
	}

	//Sort......Point PtrArray.....
	//......
	ads_printf("\nSorting point data....");
	SortAcPoint3DArray(Pt3DArray, 0);//X

	//Copy data to allocated memory to be used by Core func.
	iNoOfPts = 0;
	for(i = 0; i < Pt3DArray.length(); i++)
	{
		iNoOfPts += 1;
		*(pArrayOfPts + iNoOfPts) = (Pt3DArray[i]);//0 to 1 - Based
	}
	//************************************************************************************************
	//Call CORE Routine.....
	//************************************************************************************************
	if(1 != TriangulateCore(iNoOfPts, pArrayOfPts, pTriangles, &iNoOfTrgs))
	{
		ads_printf("\nERROR: TRIANGULATE - Calculation Error\n");
		free(pArrayOfPts);
		free(pTriangles);
		return 0;
	}
	//************************************************************************************************
	//Copy Calculated data to Ptr Array
	if(Pt3DArray.length() > 0)
		Pt3DArray.removeSubArray(0, Pt3DArray.length() - 1);
	for(i = 1; i <= iNoOfPts; i++)
	{
		AcGePoint3d PtToAdd;
		
		PtToAdd = *(pArrayOfPts + i);
		Pt3DArray.append(PtToAdd);//0  - Based
	}
	//Copy Traingle Array
	{////Reset Triangle data...
		int i;
		TRIANGLE* pRec;
		for(i = 0; i < TrgArray.GetSize(); i++)
		{
			pRec = TrgArray[i];
			delete pRec;
		}
		if(TrgArray.GetSize() > 0)
			TrgArray.RemoveAll();
	}
	for(i = 1; i <= iNoOfTrgs; i++)
	{
		TRIANGLE* pTrg;

		pTrg = new TRIANGLE;
		//1 -Based to 0-Based
		*pTrg = *(pTriangles + i);
		pTrg->p1 = pTrg->p1  - 1;
		pTrg->p2 = pTrg->p2  - 1;
		pTrg->p3 = pTrg->p3  - 1;
	
		TrgArray.Add(pTrg);
	}
	free(pArrayOfPts);
	free(pTriangles);
	ads_printf("\nTriangulation Done!");
	return 1;
}
//************************************************************************************************
//CORE Calculation.....
//************************************************************************************************
int TIN::TriangulateCore(const int& nv, AcGePoint3d*& pxyz, TRIANGLE*& v, int* ntri)
{
	const int EMAX = nv * 200;
	const int TRIMAX = nv * 200;
	int *complete;
	int nedge;
	EDGE *edges;

	int inside;
	int i,j,k;
	double xp,yp,x1,y1,x2,y2,x3,y3,xc,yc,r;
	double xmin,xmax,ymin,ymax,xmid,ymid;
	double dx,dy,dmax;

	/* Allocate memory for the edge list */
	if ((edges = (EDGE *)malloc(EMAX*(long)sizeof(EDGE))) == NULL)
	{
		ads_printf("\nERROR: TRIANGULATE - unable to allocate enough memory\n");
		return 0;
	}
	/* Allocate memory for the completeness list, flag for each triangle */
	if ((complete = (int *)malloc(TRIMAX*(long)sizeof(int))) == NULL)
	{
		ads_printf("ERROR: TRIANGULATE - unable to allocate enough memory\n");
		free(edges);
		return 0;
	}
	
	/*
	   Find the maximum and minimum vertex bounds.
	   This is to allow calculation of the bounding triangle
	*/
	xmin = pxyz[1].x;
	ymin = pxyz[1].y;
	xmax = xmin;
	ymax = ymin;

	for (i = 2; i <= nv; i++)
	{
		if (pxyz[i].x < xmin) xmin = pxyz[i].x;
		if (pxyz[i].x > xmax) xmax = pxyz[i].x;
		if (pxyz[i].y < ymin) ymin = pxyz[i].y;
		if (pxyz[i].y > ymax) ymax = pxyz[i].y;
	}
	dx = xmax - xmin;
	dy = ymax - ymin;
	dmax = (dx > dy) ? dx : dy;
	xmid = (xmax + xmin) / 2.0;
	ymid = (ymax + ymin) / 2.0;
	
	/*
	   Set up the supertriangle
	   This is a triangle which encompasses all the sample points.
	   The supertriangle coordinates are added to the end of the 
	   vertex list. The supertriangle is the first triangle in
	   the triangle list.
	*/
	pxyz[nv + 1].x = xmid - 2.0 * dmax;
	pxyz[nv + 1].y = ymid - dmax;
	pxyz[nv + 1].z = 0.0;
	pxyz[nv + 2].x = xmid;
	pxyz[nv + 2].y = ymid + 2.0 * dmax;
	pxyz[nv + 2].z = 0.0;
	pxyz[nv + 3].x = xmid + 2.0 * dmax;
	pxyz[nv + 3].y = ymid - dmax;
	pxyz[nv + 3].z = 0.0;
	v[1].p1 = nv + 1;
	v[1].p2 = nv + 2;
	v[1].p3 = nv + 3;
	complete[1] = FALSE;
	*ntri = 1;

	/*
	   Include each point one at a time into the existing mesh
	*/
	for (i=1;i<=nv;i++)
	{
		xp = pxyz[i].x;
		yp = pxyz[i].y;
		nedge = 0;

		/*
		  Set up the edge buffer. 
			  If the point (xp,yp) lies inside the CircumCircle then the
		  three edges of that triangle are added to the edge buffer
		  and that triangle is removed.
		*/
		j = 0;
		do
		{
			j++;
			if (!complete[j])
			{
				x1 = pxyz[v[j].p1].x;
				y1 = pxyz[v[j].p1].y;
				x2 = pxyz[v[j].p2].x;
				y2 = pxyz[v[j].p2].y;
				x3 = pxyz[v[j].p3].x;
				y3 = pxyz[v[j].p3].y;
				inside = CircumCircle(xp, yp, x1, y1, x2, y2, x3, y3, xc, yc, r);
				if (xc + r < xp)
	       	    complete[j] = TRUE;
	            if (inside)
				{
					/* Check that we haven't exceeded the edge list size */
					if (nedge+3 > EMAX)
					{
						ads_printf("\nERROR: TRIANGULATE - Internal edge list exceeded\n");
						free(edges);
						free(complete);
						return 0;
					}
					edges[nedge+1].p1 = v[j].p1;
					edges[nedge+1].p2 = v[j].p2;
					edges[nedge+2].p1 = v[j].p2;
					edges[nedge+2].p2 = v[j].p3;
					edges[nedge+3].p1 = v[j].p3;
					edges[nedge+3].p2 = v[j].p1;
					nedge += 3;
					v[j] = v[*ntri];
					complete[j] = complete[*ntri];
					j--;
					(*ntri)--;
				}
			}
		} while (j < *ntri);

	    /*
	        Tag multiple edges
	        Note: if all triangles are specified anticlockwise then all
	              interior edges are opposite pointing in direction.
	    */
		for(j = 1; j < nedge; j++)
		{
	        for (k = j + 1; k <= nedge; k++)
			{
				if ((edges[j].p1 == edges[k].p2) && (edges[j].p2 == edges[k].p1))
				{
					edges[j].p1 = 0;	
					edges[j].p2 = 0;
					edges[k].p1 = 0;	
					edges[k].p2 = 0;
				}
				/* Shouldn't need the following, see note above */
				if ((edges[j].p1 == edges[k].p1) && (edges[j].p2 == edges[k].p2))
				{
					edges[j].p1 = 0;	
					edges[j].p2 = 0;
					edges[k].p1 = 0;	
					edges[k].p2 = 0;
				}
			}  
	    }
	    /*
	       Form new triangles for the current point
	       Skipping over any tagged edges.
	       All edges are arranged in clockwise order.
	    */
		for (j = 1; j <= nedge; j++)
		{
			if (edges[j].p1 != 0 && edges[j].p2 != 0)
			{
				if (*ntri > TRIMAX)
				{
					ads_printf("\nERROR: TRIANGULATE - %d triangles exceeds maximum\n",*ntri);
					free(edges);
					free(complete);
					return 0;
				}
				(*ntri)++;
				v[*ntri].p1 = edges[j].p1;
				v[*ntri].p2 = edges[j].p2;
				v[*ntri].p3 = i;
				complete[*ntri] = FALSE;
			}
		}
	}
	/*
	Remove triangles with supertriangle vertices
	These are triangles which have a vertex number greater than nv
	*/
	i = 0;
	do
	{
		i++;
		if ((v[i].p1 > nv) || (v[i].p2 > nv) || (v[i].p3 > nv))
		{
			v[i] = v[*ntri];
			i--;
			(*ntri)--;
		}
	} while (i < *ntri);

	free(edges);
	free(complete);
	
	{//For output dump...
	/*
		//
		FILE* fpw;
		fpw = fopen("out.dat", "wt");
		for (i = 1; i <= *ntri; i++ )	
		{
			fprintf(fpw,"%f\t%f\t%f\t%f\t%f\t%f\n",
			pxyz[v[i].p1].x,
			pxyz[v[i].p1].y,
			pxyz[v[i].p2].x,
			pxyz[v[i].p2].y,
			pxyz[v[i].p3].x,
			pxyz[v[i].p3].y);
		}
		fclose(fpw);
	*/
	}
	ads_printf("\rTriangulation Done\n");
	return 1;
}

int TIN::CircumCircle(const double& xp, const double& yp, const double& x1, const double& y1, const double& x2, const double& y2, const double& x3, const double& y3, double& xc, double& yc, double& r)
{
	#define ABS(x) ( (x) < 0 ? -(x) : (x) )
	#define EPSILON  0.000001     /* small float for vertex closeness  */

	double m1,m2,mx1,mx2,my1,my2;
	double dx,dy,rsqr,drsqr;

	if (ABS(y1-y2) < EPSILON && ABS(y2-y3) < EPSILON)
	{
		ads_printf("\nTRIANGULATE - Coincident points\n");
		return(FALSE);
	}
	if (ABS(y2-y1) < EPSILON)
	{
		m2 = - (x3-x2) / (y3-y2);
		mx2 = (x2 + x3) / 2.0;
		my2 = (y2 + y3) / 2.0;
		xc = (x2 + x1) / 2.0;
		yc = m2 * (xc - mx2) + my2;
	}
	else if (ABS(y3-y2) < EPSILON)
	{
       	m1 = - (x2-x1) / (y2-y1);
       	mx1 = (x1 + x2) / 2.0;
       	my1 = (y1 + y2) / 2.0;
       	xc = (x3 + x2) / 2.0;
       	yc = m1 * (xc - mx1) + my1;
    }
	else
	{
       	m1 = - (x2-x1) / (y2-y1);
       	m2 = - (x3-x2) / (y3-y2);
       	mx1 = (x1 + x2) / 2.0;
       	mx2 = (x2 + x3) / 2.0;
       	my1 = (y1 + y2) / 2.0;
       	my2 = (y2 + y3) / 2.0;
       	xc = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);
       	yc = m1 * (xc - mx1) + my1;
    }
	dx = x2 - xc;
	dy = y2 - yc;
	rsqr = dx*dx + dy*dy;
	r = sqrt(rsqr);

	dx = xp - xc;
	dy = yp - yc;
	drsqr = dx*dx + dy*dy;

	return((drsqr <= rsqr) ? TRUE : FALSE);
}
