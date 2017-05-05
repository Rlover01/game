#pragma once

#include "cbase.h"
#include "filesystem.h"
#include "../mom_shareddefs.h"
#include "util/serialization.h"

#ifdef CLIENT_DLL
#define CMomRunStats C_MomRunStats
EXTERN_RECV_TABLE(DT_MOM_RunStats);
#else
EXTERN_SEND_TABLE(DT_MOM_RunStats);
#endif

/*
 * USAGE: This object is pointless without an instance of CMomRunStats::data to point to.
 * By nature of the CMomRunStats::data's purpose, this class cannot own it, but it can own
 * a pointer to it. Since this class can't make what it needs to point to, it needs some construction
 * done by the creator of the instance of the class. The methods Init() and Deserialize() rely on the
 * pointer already being set, so they can't be used until after construction.
 *      What all this means is that construction is currently handled by the caller.
 * 
 * To do this, create the class, create the data, set the pointer in the class,
 * then call either the Init(uint8) method or Deserialize(CBinaryReader *reader).
 */

class CMomRunStats : public ISerializable
{
    DECLARE_CLASS_NOBASE(CMomRunStats);
    //DECLARE_EMBEDDED_NETWORKVAR();
    
public:
    //TODO: HACK: These constructors are (probably just temporarily) deprecated. We need to either make this object
    //actually capable of constructing its self, or continue to construct it externally.
    CMomRunStats(uint8 size = MAX_STAGES);
    CMomRunStats(CBinaryReader *pReader);

    // Note: This needs updating every time the struct is updated!
    virtual void Init(uint8 size = MAX_STAGES);

    // Note: This needs updating every time the struct is updated!
    virtual void Deserialize(CBinaryReader *reader);

    // Note: This needs updating every time the struct is updated!
    void Serialize(CBinaryWriter *writer) OVERRIDE;

    // Note: This needs updating every time the struct is updated!
    CMomRunStats &operator=(const CMomRunStats &other);

  public:
    // All these are virtual so they can be overridden in future versions.
    virtual uint8 GetTotalZones();
    virtual uint32 GetZoneJumps(int zone);
    virtual uint32 GetZoneStrafes(int zone);
    virtual float GetZoneTime(int zone);
    virtual float GetZoneEnterTime(int zone);
    virtual float GetZoneStrafeSyncAvg(int zone);
    virtual float GetZoneStrafeSync2Avg(int zone);
    virtual float GetZoneEnterSpeed(int zone, bool vel2D);
    virtual float GetZoneExitSpeed(int zone, bool vel2D);
    virtual float GetZoneVelocityMax(int zone, bool vel2D);
    virtual float GetZoneVelocityAvg(int zone, bool vel2D);


    virtual void SetTotalZones(uint8 zones);
    virtual void SetZoneJumps(int zone, uint32 value);
    virtual void SetZoneStrafes(int zone, uint32 value);
    virtual void SetZoneTime(int zone, float value);
    virtual void SetZoneEnterTime(int zone, float value);
    virtual void SetZoneStrafeSyncAvg(int zone, float value);
    virtual void SetZoneStrafeSync2Avg(int zone, float value);
    virtual void SetZoneEnterSpeed(int zone, float vert, float hor);
    virtual void SetZoneVelocityMax(int zone, float vert, float hor);
    virtual void SetZoneVelocityAvg(int zone, float vert, float hor);
    virtual void SetZoneExitSpeed(int zone, float vert, float hor);

    /*
     * We encapsulate the raw data in its own struct to allow a memcpy of just the data
     * rather than a memcpy of CMomRunStats which would also copy pointers to virtuals
     */ 
    struct data 
    {
        // Note: Passing 0 as the index to any of these will return the overall stat, i.e during the entire run.
        
        uint8 m_iTotalZones; // Required for the operator= overload

        // Keypress
        uint32 m_iZoneJumps[MAX_STAGES + 1], // Amount of jumps per stage/checkpoint
            m_iZoneStrafes[MAX_STAGES + 1];  // Amount of strafes per stage/checkpoint

        // Time
        float m_flZoneTime[MAX_STAGES + 1], // The amount of time (seconds) you spent to accomplish (stage) -> (stage + 1)
            m_flZoneEnterTime[MAX_STAGES + 1]; // The time in seconds that you entered the given stage/checkpoint

        // Sync
        float m_flZoneStrafeSyncAvg[MAX_STAGES + 1], // The average sync1 you had over the given stage/checkpoint
            m_flZoneStrafeSync2Avg[MAX_STAGES + 1];  // The average sync2 you had over the given stage/checkpoint

        // Velocity
        float m_flZoneEnterSpeed3D[MAX_STAGES + 1], m_flZoneEnterSpeed2D[MAX_STAGES + 1],// The 3D velocity with which you started the stage (exit this stage's start trigger)
            m_flZoneVelocityMax3D[MAX_STAGES + 1], m_flZoneVelocityMax2D[MAX_STAGES + 1],// Max velocity for a stage/checkpoint
            m_flZoneVelocityAvg3D[MAX_STAGES + 1], m_flZoneVelocityAvg2D[MAX_STAGES + 1],// Average velocity in a stage/checkpoint
            m_flZoneExitSpeed3D[MAX_STAGES + 1], m_flZoneExitSpeed2D[MAX_STAGES + 1];// The velocity with which you exit the stage (this stage -> next)
    };
    
    //A public pointer to the data kind of undermines the encapsulation, but it is necessary
    CMomRunStats::data *m_pData;
};


