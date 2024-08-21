#include <unordered_map>
#include <set>
#include <thread>

#include <vector>

#include "EventDispatcher.h"

namespace ofsim_infrastructure 
{   
    enum class StateName
    {    
        // main states:
        WAITING_FOR_BEGIN,
        SIMULATION_RUNNIG,
        SIMULATION_PAUSED, 
        TRAJECTORY_PREDICTION, 
        DIAGNOSTICS,
        PRESENTATION,
        FINISH,

        SHOW_HELP,
        SHOW_FILE_LOAD_DIALOG,
        LOAD_FILE_FROM_DIALOG
    };

    struct SimulationState
    {
        StateName stateName;        
        StateEvent triggeredEvent;
        bool immediateState = false;
        std::unordered_map<StateEvent, StateName> tranistions;       
    };

    class StateMachine
    {
    public:
        StateMachine(EventDispatcher &_eventDispatcher);
        ~StateMachine() 
        { 
            shouldListen = false;
            stateMachineThread->join();
            stateMachineThread.reset();
        }

    private:
        EventDispatcher &eventDispatcher;

        std::unordered_map<StateName, SimulationState> stateGraph;
        std::unordered_map<StateEvent, StateName> stateTriggering;
        StateName currentState = StateName::WAITING_FOR_BEGIN;

        std::unique_ptr<std::thread> stateMachineThread;
        
        bool shouldListen = true;

        void recieveEvents();
        void processEvent(StateEvent event);
        void sendEvent(StateEvent event);
        void buildGraph();
    };
}