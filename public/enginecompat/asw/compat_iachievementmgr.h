abstract_class IAchievement
{
public:
	virtual int GetAchievementID() = 0;
	virtual const char *GetName() = 0;
	virtual int GetFlags() = 0;
	virtual int GetGoal() = 0;
	virtual int GetCount() = 0;
	virtual bool IsAchieved() = 0;
	virtual int GetPointValue() = 0;
	virtual bool ShouldSaveWithGame() = 0;
	virtual bool ShouldHideUntilAchieved() = 0;
	virtual const char *GetIconPath() = 0;
	virtual int GetDisplayOrder() = 0;
};


abstract_class IAchievementMgr
{
public:
	virtual IAchievement* GetAchievementByIndex( int index, int nPlayerSlot ) = 0;
	virtual IAchievement* GetAchievementByDisplayOrder( int orderIndex, int nPlayerSlot ) = 0;
	virtual CBaseAchievement* GetAchievementByID ( int id, int nPlayerSlot ) = 0;
	virtual int GetAchievementCount() = 0;
	virtual void InitializeAchievements( ) = 0;
	virtual void AwardAchievement( int nAchievementID, int nPlayerSlot ) = 0;
	virtual void OnMapEvent( const char *pchEventName, int nPlayerSlot ) = 0;
	virtual void SaveGlobalStateIfDirty( ) = 0;
	virtual bool HasAchieved( const char *pchName, int nPlayerSlot ) = 0;
	virtual const CUtlVector<int>& GetAchievedDuringCurrentGame( int nPlayerSlot ) = 0;
}; 
