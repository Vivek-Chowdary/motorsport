/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef SYSTEM_HPP
#   define SYSTEM_HPP
#   include "SDL/SDL_types.h"
#   include <map>
#   include "axis.hpp"

namespace Ogre {
class RenderWindow;
class SceneManager;
}
class SystemData;
/** 
 * @brief Contains a lot of data, generally needed in many places. It's a singleton.
 */
class SystemData
{
    /** 
     * @brief The only SystemData object instance (singleton).
     */
    static SystemData systemDataObject;
    /** 
     * @brief The main loop will keep running while this is set to true.
     */
    bool mainLoopEnabled;
    /** 
     * @brief CFM value used by ODE.
     */
    double cfmValue;     
    /** 
     * @brief ERP value used by ODE.
     */
    double erpValue;
    
    /** 
     * @brief The desired number of times per second (Hz) the physics engine should be run.
     */
    double desiredPhysicsFrequency;
    /** 
     * @brief The actual number of times per second the physics engine is running.
     */
    double currentPhysicsFrequency;
  public:
    /** 
     * @brief Returns the SystemData singleton instance.
     * 
     * @return A SystemData object instance.
     */
    static SystemData *getSystemDataPointer ();
    /** 
     * @brief Initializes the instance some default hardcoded values.
     */
    SystemData ();
    /** 
     * @brief Destroys the current SystemData instance.
     */
    ~SystemData ();

    /** 
     * @brief Returns the ODE CFM value.
     * 
     * @return The CFM value.
     */
    double getCfmValue();
    /** 
     * @brief Sets the ODE CFM value.
     * 
     * @param value The CFM value.
     */
    void setCfmValue(const double & value);
    /** 
     * @brief Returns the ODE ERP value.
     * 
     * @return The ERP value.
     */
    double getErpValue();
    /** 
     * @brief Sets the ODE ERP value.
     * 
     * @param value The ERP value.
     */
    void setErpValue(const double & value);
    
    /** 
     * @brief Get the desired physics frequency.
     * 
     * @return The frequency in Hz.
     */
    double getDesiredPhysicsFrequency();
    /** 
     * @brief Get the desired physics timestep (1/frequency).
     * 
     * @return The timestep in seconds.
     */
    double getDesiredPhysicsTimestep();
    /** 
     * @brief Sets the desired physics frequency.
     * 
     * @param frequency The frequency in Hz.
     */
    void setDesiredPhysicsFrequency(double frequency);
    /** 
     * @brief Sets the desired physics timestep.
     * 
     * @param timestep The timestep in seconds.
     */
    void setDesiredPhysicsTimestep(double timestep);
    /** 
     * @brief Increases the physics rate by an arbitrary value, more or less proportional to current frequency.
     */
    void increaseDesiredPhysicsRate();
    /** 
     * @brief Decreases the physics rate by an arbitrary value, more or less proportional to current frequency.
     */
    void decreaseDesiredPhysicsRate();

    /** 
     * @brief Get the current physics frequency.
     * 
     * @return The frequency in Hz.
     */
    double getCurrentPhysicsFrequency();
    /** 
     * @brief Get the current physics timestep (1/frequency).
     * 
     * @return The timestep in seconds.
     */
    double getCurrentPhysicsTimestep();
    /** 
     * @brief Update the current physics frequency value. To be used for statistics.
     * 
     * @param frequency The new frequency in Hz.
     */
    void setCurrentPhysicsFrequency(double frequency);
    /** 
     * @brief Update the current physics timestep value. To be used for statistics.
     * 
     * @param timestep The new timestep in seconds.
     */
    void setCurrentPhysicsTimestep(double timestep);

    // main loop time control data
    /** 
     * @brief Current wall time, measured in seconds.
     */
    double realTime;
    /** 
     * @brief Current sim time, measured in seconds. Tries to catch the realTime.
     */
    double simulationTime;
    /** 
     * @brief Last time statistics where taken. In seconds.
     */
    double statisticsTime;
    /** 
     * @brief Number of physics steps computed. Used for statistics.
     */
    double physicsSteps;
    /** 
     * @brief Number of graphics steps computed. Used for statistics.
     */
    double graphicsSteps;
    /** 
     * @brief Current graphics frequency, measured in Hz. To be used for statistics.
     */
    double graphicsFrequency;

    /** 
     * @brief Physics time scale. 1=normal speed. 2=slow motion (1/2 the normal speed). 3=slow motion (1/3 the normal speed).
     */
    int timeScale;
    /** 
     * @brief Physics step when the physics will stop to be computed. Dependent on physics rate!
     */
    int pauseStep;
    /** 
     * @brief Screen vertical resolution.
     */
    int height;
    /** 
     * @brief Screen horizontal resolution.
     */
    int width;

    /** 
     * @brief Timestep for video recording, in milliseconds. A screen frame will be recorded every videoRecordTimested seconds.
     */
    int videoRecordTimestep;
    /** 
     * @brief Returns whether the main loop has to keep running.
     * 
     * @return true if main loop has to keep running.
     * @return false if main loop has to be stopped.
     */
    bool isMainLoopEnabled (void); // does the program have to keep running?
    /** 
     * @brief Notifies the system to allow the main loop to run.
     */
    void enableMainLoop (void); // allows the program to start running
    /** 
     * @brief Notifies the sytem that the main loop should be stopped.
     */
    void disableMainLoop (void);    // allows the program to stop running

    Ogre::RenderWindow * ogreWindow;
    Ogre::SceneManager * ogreSceneManager;
    std::map <int, Axis * > axisMap;

    bool cameraDirector;
};

#endif
