import ReactRadialGauge  from "./ReactRadialGauge";

interface ICO2RadialGaugeProps {
    Value : number;
}

function CO2RadialGauge(props : ICO2RadialGaugeProps) {
    return (<ReactRadialGauge
        renderTo=""
        minValue={400}
        maxValue={2000}
        value={props.Value}
        units={false}
        exactTicks={true}
        majorTicks={[
                400,
                700,
                1000,
                1500,
                2000,       
            ]}
        minorTicks={0}
        strokeTicks={true}
        animatedValue={true}
        animateOnInit={false}
        title="CO2 (ppm)"
        borders={true}
        numbersMargin={1}
        valueInt={3}
        valueDec={0}
        majorTicksInt={1}
        majorTicksDec={0}
        animation={true}
        animationDuration={500}
        animationRule="cycle"
        colorPlate="#fff"
        colorPlateEnd=""
        colorMajorTicks="#444"
        colorMinorTicks="#666"
        colorTitle="#888"
        colorUnits="#888"
        colorNumbers="#444"
        colorNeedle="rgba(240,128,128,1)"
        colorNeedleEnd="rgba(255,160,122,.9)"
        colorValueText= "#444"
        colorValueTextShadow="rgba(0,0,0,0.3)"
        colorBorderShadow="rgba(0,0,0,0.5)"
        colorBorderOuter="#ddd"
        colorBorderOuterEnd="#aaa"
        colorBorderMiddle="#eee"
        colorBorderMiddleEnd ="#f0f0f0"
        colorBorderInner = "#fafafa"
        colorBorderInnerEnd = "#ccc"
        colorValueBoxRect = "#888"
        colorValueBoxRectEnd = "#666"
        colorValueBoxBackground = "#babab2"
        colorValueBoxShadow = "rgba(0,0,0,1)"
        colorNeedleShadowUp = "rgba(2,255,255,0.2)"
        colorNeedleShadowDown = "rgba(188,143,143,0.45)"
        colorBarStroke = "#222"
        colorBar = "#ccc"
        colorBarProgress = "#888"
        colorBarShadow = "#000"
        fontNumbers = "Arial"
        fontTitle = "Arial"
        fontUnits = "Arial"
        fontValue = "Arial"
        fontNumbersSize = {20}
        fontTitleSize = {24}
        fontUnitsSize = {22}
        fontValueSize = {26}
        fontNumbersStyle = "normal"
        fontTitleStyle = "normal"
        fontUnitsStyle = "normal"
        fontValueStyle = "normal"
        fontNumbersWeight = "normal"
        fontTitleWeight = "normal"
        fontUnitsWeight = "normal"
        fontValueWeight = "normal"
        needle = {true}
        needleShadow = {true}
        needleType = "arrow"
        needleStart = {5}
        needleEnd = {85}
        needleWidth = {4}
        borderOuterWidth = {3}
        borderMiddleWidth = {3}
        borderInnerWidth = {3}
        borderShadowWidth = {3}
        valueBox = {true}
        valueBoxStroke = {5}
        valueBoxWidth = {0}
        valueText = ""
        valueTextShadow = {true}
        valueBoxBorderRadius = {2.5}
        highlights = {[
            {
                "from": 400,
                "to": 700,
                "color": "#1bad13"
            },
            {
                "from": 701,
                "to": 1000,
                "color": "#adaa13"
            },
            {
                "from": 1001,
                "to": 1500,
                "color": "#d47400"
            },
            {
                "from": 1500,
                "to": 2000,
                "color": "#d40000"
            }
        ]}
        highlightsWidth = {15}
        barWidth = {0}
        barStrokeWidth = {0}
        barProgress = {true}
        barShadow = {0}
        ticksAngle = {270}
        startAngle = {45}
        colorNeedleCircleOuter = "#f0f0f0"
        colorNeedleCircleOuterEnd = "#ccc"
        colorNeedleCircleInner = "#e8e8e8"
        colorNeedleCircleInnerEnd = "#f5f5f5"
        needleCircleSize = {10}
        needleCircleInner = {true}
        needleCircleOuter = {true}
        animationTarget = "needle"
        useMinPath = {false} />)
}


export default CO2RadialGauge;