#ifndef ARRAY_MGR_H__8096FEFA_E655_46ED_9D17_03C9AB4DFD83__INCLUDED_
#define ARRAY_MGR_H__8096FEFA_E655_46ED_9D17_03C9AB4DFD83__INCLUDED_

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "GMaster.h"
#include "CCriticalLock.h"

#define ARRAY_ID_SIZE			64

#define ARRAY_ORDER_DIRECT		0x01
#define ARRAY_ORDER_FIFO		0x02

// ========================
// ** EXTERNAL FUNCTIONS **
// ========================

typedef void	(*ARRAY_DESTROY) (void *p_pstArray, void *p_pstData);


// ======================
//  ARRAY DATA STRUCTURE
// ======================

typedef struct ARRAY_DATA_T
{
	int				iIndex;
	
	unsigned char	ucFlags;

	// Flag definitions
	// 0 0 0 0 0 0 0 0
	// | | |_|_|_|_|_|__________________________________________________ For furture use
	// | |______________________________________________________________ Delete flag - 0: dont delete, 1: Can delete
	// |________________________________________________________________ Used flag - 0: Free, 1: Occupied
	
	char			acID [ARRAY_ID_SIZE + 1];

	void			*pvData;
}
ARRAY_DATA;

typedef struct ARRAY_LIST_T
{
	int				iSize,
					iCount,
					iHeadIndex,
					iTailIndex;

	unsigned char	ucFlags,
					ucOrder;

	// Flag definitions
	// 0 0 0 0 0 0 0 0
	// | | |_|_|_|_|_|__________________________________________________ For furture use
	// | |______________________________________________________________ Duplicate flag - 0: not allow duplicate, 1: allow duplicate
	// |________________________________________________________________ Initialize flag - 0: not initialize, 1: initialized

	CRITICAL_LOCK	stLock;
	
	ARRAY_DESTROY	pfDestroy;

	ARRAY_DATA		*pastList;
}
ARRAY_LIST;

// =====================
// ** PUBLIC FUNCTION **
// =====================

unsigned char	array_init (ARRAY_LIST *p_pstArray, int p_iSize, unsigned char p_ucOrder, unsigned char p_ucAllowDuplicate, ARRAY_DESTROY p_pfDestroy);
void			array_term (ARRAY_LIST *p_pstArray);

unsigned char	array_lock (ARRAY_LIST *p_pstArray);
unsigned char	array_unlock (ARRAY_LIST *p_pstArray);

unsigned char	array_add (ARRAY_LIST *p_pstArray, void *p_pvData, char *p_pcID, unsigned char p_ucDelFlag, ARRAY_DATA **p_ppstItem);
unsigned char	array_del (ARRAY_LIST *p_pstArray, void **p_ppvData);
unsigned char	array_del_by_index (ARRAY_LIST *p_pstArray, int p_iIndex, void **p_ppvData);
unsigned char	array_del_by_id (ARRAY_LIST *p_pstArray, char *p_pcID, void **p_ppvData);
void			array_del_all (ARRAY_LIST *p_pstArray);

unsigned char	array_get_by_index (ARRAY_LIST *p_pstArray, int p_iIndex, ARRAY_DATA **p_ppstArrayData);
unsigned char	array_get_by_id (ARRAY_LIST *p_pstArray, char *p_pcID, ARRAY_DATA **p_ppstArrayData);

unsigned char	array_get_first (ARRAY_LIST *p_pstArray, ARRAY_DATA **p_ppstNextData);
unsigned char	array_get_next (ARRAY_LIST *p_pstArray, ARRAY_DATA *p_pstCurData, ARRAY_DATA **p_ppstNextData);

#endif
