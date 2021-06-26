#pragma once
class IClientEntityList
{
public:
	// Get IClientNetworkable interface for specified entity
	virtual IClientNetworkable*	GetClientNetworkable(int entnum) = 0;
	virtual IClientNetworkable*	GetClientNetworkableFromHandle(void* hEnt) = 0;
	virtual void*		GetClientUnknownFromHandle(void* hEnt) = 0;

	// NOTE: This function is only a convenience wrapper.
	// It returns GetClientNetworkable( entnum )->GetIClientEntity().
	virtual IClientEntity*		GetClientEntity(int entnum) = 0;
	virtual IClientEntity*		GetClientEntityFromHandle(CBaseHandle hEnt) = 0;

	// Returns number of entities currently in use
	virtual int					NumberOfEntities(bool bIncludeNonNetworkable) = 0;

	// Returns highest index actually used
	virtual int					GetHighestEntityIndex(void) = 0;

	// Sizes entity list to specified size
	virtual void				SetMaxEntities(int maxents) = 0;
	virtual int					GetMaxEntities() = 0;
};