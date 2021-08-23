import React, { Component } from 'react';
import IotReciever from './components/iot-reciever-aws';
import autoBind from 'react-autobind';
import GaugeChart from 'react-gauge-chart'
import Hack from "./assets/hackster.png"
import Aws from "./assets/aws.png"
import { isMobile } from "react-device-detect"
import "./App.css"

class App extends Component {
  constructor(props) {
    super(props);
    autoBind(this);
    this.state = {
      number: 0,
      devState: "Offline",
      devStateColor: "#FF0000"
    }
  }

  componentDidMount() {

  }

  componentWillUnmount() {

  }

  callback(data) {
    if (this.state.number !== (Object.keys(JSON.parse(data[1])).length / 10)) {
      if ((Object.keys(JSON.parse(data[1])).length / 10) >= 1) {
        this.setState({
          number: 1,
          devStateColor: "#00FF00",
          devState: "Online"
        });
      }
      else {
        this.setState({
          number: (Object.keys(JSON.parse(data[1])).length / 10),
          devStateColor: "#00FF00",
          devState: "Online"
        });
      }

    }
  }


  render() {
    let styles = {
      chart: {
        width: "70%"
      },
      status: {
      },
      theZone:{
        textAlign: "center"
      }
    }
    if (isMobile) {
      styles = {
        chart: {
          width: "100%"
        },
        status: {
          position: "absolute",
          right: 0
        },
        theZone:{
          textAlign: "center",
          paddingTop:"15vh"
        }
      }
    }
    return (
      <div>
        <IotReciever sub_topics={["/TheZone/M5Core"]} callback={this.callback} />
        <h1 style={styles.status}>
          Device Status:
          <div style={{ color: this.state.devStateColor }}>
            {this.state.devState}
          </div>
          <hr></hr>
        </h1>
        <h1 style={styles.theZone}>
          The Zone
        </h1>
        <h3 style={{ textAlign: "center" }}>
          Powered by AWS
        </h3>
        <div className="centerDiv">
          <GaugeChart
            id="gauge-chart1"
            textColor="#000000"
            percent={this.state.number}
            animate={true}
            style={styles.chart}
            arcsLength={[0.3, 0.4, 0.3]}
          />
        </div>
        <hr></hr>
        {
          isMobile ?
            <>
              <div className="centerDiv">
                <img alt="logoHack" src={Hack} style={{ height: "10vh" }} />
              </div>
              <br />
              <div className="centerDiv">
                <img alt="logoAws" src={Aws} style={{ height: "10vh" }} />
              </div>
            </> :
            <>
              <img alt="logoHack" src={Hack} className="footerR" />
              <img alt="logoAws" src={Aws} className="footerL" />
            </>
        }
      </div>
    );
  }
}

export default App;