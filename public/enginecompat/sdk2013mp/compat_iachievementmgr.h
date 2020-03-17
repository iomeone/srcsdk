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
	virtual bool ShouldShowOnHUD() = 0;
	virtual void SetShowOnHUD( bool bShow ) = 0;
};


abstract_class IAchievementMgr
{
public:
	virtual IAchievement* GetAchievementByIndex( int index ) = 0;
	virtual CBaseAchievement* GetAchievementByID ( int id ) = 0;
	virtual int GetAchievementCount() = 0;
	virtual void InitializeAchievements() = 0;
	virtual void AwardAchievement( int iAchievementID ) = 0;
	virtual void OnMapEvent( const char *pchEventName ) = 0;
	virtual void DownloadUserData() = 0;
	virtual void EnsureGlobalStateLoaded() = 0;
	virtual void SaveGlobalStateIfDirty( bool bAsync ) = 0;
	virtual bool HasAchieved( const char *pchName ) = 0;
	virtual bool WereCheatsEverOn() = 0;
}; 
