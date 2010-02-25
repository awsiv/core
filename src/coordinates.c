
/*

 This file is (C) Cfengine AS. See LICENSE for details.

*/

#include "cf3.defs.h"
#include "cf3.extern.h"
#include "cf.nova.h"

/******************************************************************/
/*                                                                */
/* File: coordinates.c                                            */
/*                                                                */
/******************************************************************/

int Nova_ViewPortY(struct CfDataView *cfv,double y,double bottom_margin)

{
return cfv->origin_y - (int)((y + bottom_margin - cfv->min)*cfv->scale_y);
}

/******************************************************************/

int Nova_ViewPortX(struct CfDataView *cfv,double x)

{
return cfv->origin_x + (x * cfv->scale_x);
}


/******************************************************************/

int Nova_ViewScaleX(struct CfDataView *cfv,double x)

{
return (x * cfv->scale_x);
}


/******************************************************************/

int Nova_ViewScaleY(struct CfDataView *cfv,double y)

{
return (y * cfv->scale_y);
}


