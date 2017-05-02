var https = require('https') //include https

exports.handler = (event, context) => {

  try {

    if (event.session.new) {
      // New Session
      console.log("NEW SESSION") //log this for debugging
    }

    switch (event.request.type) {

      case "LaunchRequest":
        // Launch Request
        console.log(`LAUNCH REQUEST`)
        context.succeed(
          generateResponse(
            buildSpeechletResponse("Welcome to the ForceTronics Home Automation Skill, say turn light on or turn light off", true), //response for Alexa if you just call the skill without intent
            {}
          )
        )
        break;

      case "IntentRequest":
        // Intent Request
        console.log(`INTENT REQUEST`)

        switch(event.request.intent.name) { //switch statement to select the right intent
          case "TurnLightOn":
            var endpoint = "https://data.sparkfun.com/input/YourPublicKey?private_key=YourPrivateKey&lightstate=1" //https string to log data to phant phant
            https.get(endpoint, function (result) { //use https get request to send data to phant
            console.log('Success, with: ' + result.statusCode);
            context.succeed(
              generateResponse( //if you succeeded allow Alexa to tell you state of light
                buildSpeechletResponse("The light is turned on", true),
                {}
              )
            )
            }).on('error', function (err) {
              console.log('Error, with: ' + err.message);
              context.done("Failed");
            });
            break;

          case "TurnLightOff": //the turn light off intent
            var endpoint2 = "https://data.sparkfun.com/input/YourPublicKey?private_key=YourPrivateKey&lightstate=0" // phant string to set light state to off
            https.get(endpoint2, function (result) {
            console.log('Success, with: ' + result.statusCode);
            context.succeed(
              generateResponse( //Alexa response if successful
                buildSpeechletResponse("The light is turned off", true),
                {}
              )
            )
            }).on('error', function (err) {
              console.log('Error, with: ' + err.message);
              context.done("Failed");
            });
            break;

          default:
            throw "Invalid intent"
        }
        break;

      case "SessionEndedRequest":
        // Session Ended Request
        console.log(`SESSION ENDED REQUEST`)
        break;

      default:
        context.fail(`INVALID REQUEST TYPE: ${event.request.type}`)
    }

  } catch(error) { context.fail(`Exception: ${error}`) }

}

// builds an Alexa response
buildSpeechletResponse = (outputText, shouldEndSession) => {

  return {
    outputSpeech: {
      type: "PlainText",
      text: outputText
    },
    shouldEndSession: shouldEndSession
  }

}

//plays Alexa reponse
generateResponse = (speechletResponse, sessionAttributes) => {

  return {
    version: "1.0",
    sessionAttributes: sessionAttributes,
    response: speechletResponse
  }

}
