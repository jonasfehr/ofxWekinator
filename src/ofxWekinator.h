//
//  ofxWekinator.h
//  Simple class allowing easyer setup communicating with wekinator using parameterGroups (ofxGui)
//
//  Created by Jonas Fehr on 16/12/17.
//
//

#include "ofxOsc.h"


#ifndef ofxWekinator_h
#define ofxWekinator_h

#define HOST "localhost"
#define SEND_PORT 6448
#define RECEIVE_PORT 12000
#define WEK_IN_ADDRESS "/wek/inputs"
#define WEK_OUT_ADDRESS "/wek/outputs"


class ofxWekinator{
public:
    ofxOscSender sender;
    ofxOscReceiver receiver;

    vector<ofParameterGroup*> outParameterGroups;
    ofParameterGroup * inParameters;
    
    bool bIsRunning;
    
    void setup(ofParameterGroup * inParameters, vector<ofParameterGroup*> outParameterGroups){
        this->inParameters = inParameters;
        this->outParameterGroups = outParameterGroups;
    
        sender.setup("127.0.0.1", SEND_PORT);
        receiver.setup(RECEIVE_PORT);
        
        bIsRunning = false;
        
        int numOfOutputs = 0;
        for(auto & outParameterGroup : outParameterGroups){
            numOfOutputs += outParameterGroup->size();
        }
        
        cout << "Wekinator setup: " << inParameters->size() << " inputs | " << numOfOutputs << " outputs" << endl;
    }
    
    void update(){
        if( ofGetFrameNum()%6 == 0) send();
        
        if(bIsRunning){
            receive();
            
        }else{
            if( ofGetFrameNum()%6 == 0) sendOutputs();
            
        }

        
    }
    
    void receive(){
        while(receiver.hasWaitingMessages()){
            // get the next message
            ofxOscMessage m;
            receiver.getNextMessage(&m);
            
            // check for mouse moved message
            if(m.getAddress() == WEK_OUT_ADDRESS){
                int i = 0;
                for(auto & outParameterGroup : outParameterGroups){
                    outParameterGroup->getFloat(i) = m.getArgAsFloat(i);
                    i++;
                }
            }
            else{
                // unrecognized message: display on the bottom of the screen
                string msg_string;
                msg_string = m.getAddress();
                msg_string += ": ";
                for(int i = 0; i < m.getNumArgs(); i++){
                    // get the argument type
                    msg_string += m.getArgTypeName(i);
                    msg_string += ":";
                    // display the argument - make sure we get the right type
                    if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                        msg_string += ofToString(m.getArgAsInt32(i));
                    }
                    else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                        msg_string += ofToString(m.getArgAsFloat(i));
                    }
                    else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                        msg_string += m.getArgAsString(i);
                    }
                    else{
                        msg_string += "unknown";
                    }
                }
                ofWarning("ofxWekinator: received unknown OSC message - " + msg_string );
            }
            
        }

    };

    
    void send(){
        ofxOscMessage m;
        m.setAddress(string(WEK_IN_ADDRESS));
        for(int i = 0; i < inParameters->size(); i++){
            m.addFloatArg(inParameters->getFloat(i));
        }
        sender.sendMessage(m, false);
    }   
    
    // remote control
    
    void startRecording() {
        string address = "/wekinator/control/startRecording";
        // cout << "Wekinator - Recording" << endl;
        ofxOscMessage m;
        m.setAddress(address);
        sender.sendMessage(m, false);
        bIsRunning = false;

    }
    
    void stopRecording() {
        string address = "/wekinator/control/stopRecording";
        // cout << "Wekinator - Stoping recording" << endl;
        ofxOscMessage m;
        m.setAddress(address);
        sender.sendMessage(m, false);
        bIsRunning = false;

    }
    
    void train() {
        string address = "/wekinator/control/train";
        // cout << "Wekinator - Recording" << endl;
        ofxOscMessage m;
        m.setAddress(address);
        sender.sendMessage(m, false);    }
    
    void startRunning() {
        string address = "/wekinator/control/startRunning";
        // cout << "Wekinator - Running" << endl;
        ofxOscMessage m;
        m.setAddress(address);
        sender.sendMessage(m, false);
        
        bIsRunning = true;
    }
    
    void stopRunning() {
        string address = "/wekinator/control/stopRunning";
        // cout << "Wekinator - Stop running" << endl;
        ofxOscMessage m;
        m.setAddress(address);
        sender.sendMessage(m, false);
        
        bIsRunning = false;
    }
    
    void deleteTraining() {
        string address = "/wekinator/control/deleteAllExamples";
        // cout << "Wekinator - Deleting examples" << endl;
        ofxOscMessage m;
        m.setAddress(address);
        sender.sendMessage(m, false);
    }
    
    void sendOutputs() {
        string address = "/wekinator/control/outputs";
        // cout << "Wekinator - send outputs" << endl;
        ofxOscMessage m;
        m.setAddress(address);
        for(auto & outParameterGroup : outParameterGroups){
            for(int i = 0; i < outParameterGroup->size(); i++){
                m.addFloatArg(outParameterGroup->getFloat(i));
            }
        }
        sender.sendMessage(m, false);
    }
    
    void sendInputs() {
        string address = "/wekinator/control/inputs";
        // cout << "Wekinator - send inputs" << endl;
        ofxOscMessage m;
        m.setAddress(address);
        for(int i = 0; i < inParameters->size(); i++){
            m.addFloatArg(inParameters->getFloat(i));
        }
        sender.sendMessage(m, false);
    }
    
//    void setNamesInWekinator()
//    /wekinator/control/setInputNames (with list of strings) : Sets the names for all inputs. Also changes the number of inputs to match the size of this list of strings. Note: currently this only works if Wekinator is still on the setup screen.
//    /wekinator/control/setOutputNames (with list of strings) : Sets the names for all outputs. Also changes the number of outputs to match the size of this list of strings. Note: currently this only works if Wekinator is still on the setup screen.
};

#endif /* ofxWekinator_h */
