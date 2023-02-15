import ReactRadialGauge  from "./ReactRadialGauge";
import {MajorTicks} from 'canvas-gauges'
interface IMinorRadialGaugeProps {
    Value : number;
    From : number;
    To : number;
    MajorTicks : MajorTicks;
    MinorTicks : number
    Title : string;
    Units : string;
    Decimals : number;
}

function MinorRadialGauge(props : IMinorRadialGaugeProps) {
    return (<ReactRadialGauge
        renderTo=""
        startAngle={70}
        ticksAngle={220}
        colorPlate="#ffffff"
        colorUnits="#3CA7DB"
        colorNumbers = "#3CA7DB"
        needleType = "arrow"
        needleStart = {0}
        needleEnd = {75}
        needleWidth = {4}
        needleCircleSize = {10}
        needleCircleInner = {false}
        needleCircleOuter={true}
        needleShadow={false}
        colorNeedle = "#3CA7DB"
        colorNeedleEnd = "#2698CE" 
        colorNeedleCircleOuter = "#3CA7DB" 
        colorNeedleCircleOuterEnd = "#3CA7DB"
        colorMajorTicks = "#A8D3D5"  //{["#A8D3D5", "#ffffff", "#ffffff", "#ffffff", "#ffffff",
        //    "#ffffff", "#ffffff", "#ffffff", "#ffffff", "#ffffff", "#ffffff", "#ffffff", "#ffffff", "#ffffff", "#A8D3D5"]}
        colorMinorTicks = "#ffffff"
        minValue = {props.From}
        maxValue = {props.To}
        majorTicks = { props.MajorTicks}
        minorTicks = {props.MinorTicks}
        strokeTicks = {true}
        title = {props.Title}
        highlights = {[
            {
                "from": props.From,
                "to": props.To,
                "color": "#A8D3D5"
            }
        ]}
        highlightsWidth = {25}
        numbersMargin = {12}
        animation = {true}
        animationRule = {"linear"}
        valueBox = {false}
        borders = {false}
        borderShadowWidth = {0}
        value = {props.Value}
        animateOnInit = {false}
        animatedValue  = {true}
         />)
}

export default MinorRadialGauge;