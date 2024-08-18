#include "StateMachine.h"

namespace ofsim_infrastructure
{
    StateMachine::StateMachine(EventDispatcher &_eventDispatcher) 
        : eventDispatcher(_eventDispatcher)
    {
        buildGraph();
        stateMachineThread = std::make_unique<std::thread>(&StateMachine::recieveEvents, this);
    }

    void StateMachine::recieveEvents()
    {    
        while (shouldListen)
        {
            auto event = eventDispatcher.recieveGUIEvent();
            if (event != StateEvent::NONE) 
            {
                processEvent(event);
            }
        }
    }

    void StateMachine::processEvent(StateEvent event)
    {
        if (!stateTriggering.count(event))
        {
            return;
        }

        auto stateName = stateTriggering[event];
        if (stateGraph[currentState].tranistions.count(stateName))
        {
            currentState = stateName;
            sendEvent(stateGraph[currentState].triggeredEvent);
        }
    }

    void StateMachine::sendEvent(StateEvent event)
    {
        eventDispatcher.sendSMEvent(event);
    }

    void StateMachine::buildGraph()
    {  
        stateGraph[StateName::WAITING_FOR_BEGIN] = SimulationState {
            StateName::WAITING_FOR_BEGIN,
            StateEvent::NONE,
            false,
            {
                {StateName::SIMULATION_RUNNIG},
                {StateName::SHOW_FILE_LOAD_DIALOG}                
            }
        };

        stateGraph[StateName::SIMULATION_RUNNIG] = SimulationState{
            StateName::SIMULATION_RUNNIG,
            StateEvent::SIMULATION_STARTED,
            false,            
            {
                {StateName::TRAJECTORY_PREDICTION},
                {StateName::DIAGNOSTICS},
                {StateName::PRESENTATION}
            }
        };

        stateGraph[StateName::SIMULATION_RUNNIG] = SimulationState{
            StateName::SIMULATION_RUNNIG,
            StateEvent::SIMULATION_STARTED,
            false,            
            {
                {StateName::TRAJECTORY_PREDICTION},
                {StateName::DIAGNOSTICS},
                {StateName::PRESENTATION}
            }
        };
    }
}