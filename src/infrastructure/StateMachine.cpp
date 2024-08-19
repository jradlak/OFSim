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
        if (stateGraph[currentState].tranistions.count(event) != 0)
        {            
            currentState = stateGraph[currentState].tranistions[event];
            sendEvent(stateGraph[currentState].triggeredEvent);
            if (stateGraph[currentState].immediateState)
            {
                currentState = (*stateGraph[currentState].tranistions.begin()).second;
                sendEvent(stateGraph[currentState].triggeredEvent);
            }
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
                {StateEvent::START_SIMULATION, StateName::SIMULATION_RUNNIG},
                {StateEvent::OPEN_FILE_DIALOG, StateName::SHOW_FILE_LOAD_DIALOG}
            }
        };

        stateGraph[StateName::SIMULATION_RUNNIG] = SimulationState{
            StateName::SIMULATION_RUNNIG,                        
            StateEvent::SIMULATION_STARTED,
            false,            
            {
                {StateEvent::SWITCH_TO_TRAJECTORY_PREDICTION, StateName::TRAJECTORY_PREDICTION},
                {StateEvent::SWITCH_TO_DIAGNOSTICS, StateName::DIAGNOSTICS},
                {StateEvent::SWITCH_TO_PRESENTATION, StateName::PRESENTATION},
                {StateEvent::SHOW_HELP_DIALOG,StateName::SHOW_HELP}
            }
        };

        stateGraph[StateName::TRAJECTORY_PREDICTION] = SimulationState{
            StateName::TRAJECTORY_PREDICTION,            
            StateEvent::SHOW_TRAJECTORY_PREDICTION,
            false,            
            {
                {StateEvent::CONTINUE_SIMULATION, StateName::SIMULATION_RUNNIG}                
            }
        };

        stateGraph[StateName::SHOW_FILE_LOAD_DIALOG] = SimulationState{
            StateName::SHOW_FILE_LOAD_DIALOG,            
            StateEvent::SHOW_FILE_OPEN_DIALOG,
            false,
            {                
                {StateEvent::LOAD_SELECTED_FILE, StateName::WAITING_FOR_BEGIN},
                {StateEvent::LOAD_DIRECTORY_FILES, StateName::SHOW_FILE_LOAD_DIALOG}
            }
        };        
    }
}