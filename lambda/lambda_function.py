import boto3
import json
import urllib.request
import os
import time
from datetime import datetime, timezone

cloudwatch = boto3.client('cloudwatch', region_name='ap-south-1')

IFTTT_KEY   = os.environ.get('IFTTT_KEY', '')
IFTTT_ALERT = "fire_gas_alert"
IFTTT_CLEAR = "fire_gas_allclear"

last_alert_time = 0
ALERT_COOLDOWN  = 300
was_in_alert    = False


def lambda_handler(event, context):
    print("Event received:", json.dumps(event))
    return handle_sensor_data(event, context)


def handle_sensor_data(event, context):
    global last_alert_time, was_in_alert

    flame          = int(event.get('flame', 1))
    gas_analog     = int(event.get('gas_analog', 0))
    gas_detect     = int(event.get('gas_detected', 0))
    warning_active = int(event.get('warning_active', 0))
    alert          = str(event.get('alert', ''))

    flame_status = 1 if flame == 0 else 0
    is_alert     = (gas_detect == 1 or flame == 0)


    cloudwatch.put_metric_data(
        Namespace='FireGasMonitor',
        MetricData=[
            {
                'MetricName': 'GasAnalogLevel',
                'Value': float(gas_analog),
                'Unit': 'None'
            },
            {
                'MetricName': 'FlameStatus',
                'Value': float(flame_status),
                'Unit': 'None'
            },
            {
                'MetricName': 'GasDetected',
                'Value': float(gas_detect),
                'Unit': 'None'
            },
            {
                'MetricName': 'AlertActive',
                'Value': 1.0 if is_alert else 0.0,
                'Unit': 'None'
            },
            {
                'MetricName': 'WarningActive',
                'Value': float(warning_active),
                'Unit': 'None'
            }
        ]
    )
    print(f"✓ Metrics → GasAnalog={gas_analog} | Flame={flame_status} | GasDetected={gas_detect} | Warning={warning_active} | Alert={1 if is_alert else 0}")

    current_time = int(time.time())

  
    if is_alert:
        if current_time - last_alert_time > ALERT_COOLDOWN:
            if IFTTT_KEY:
                send_ifttt_alert(alert, gas_analog, flame)
            last_alert_time = current_time
            was_in_alert    = True
            print("✓ Alert sent")
        else:
            remaining = ALERT_COOLDOWN - (current_time - last_alert_time)
            print(f"Cooldown active — next alert in {remaining}s")

   
    elif was_in_alert and not is_alert and not warning_active:
        if IFTTT_KEY:
            send_ifttt_allclear(gas_analog)
        was_in_alert = False
        print("✓ All Clear sent")

    return {'statusCode': 200, 'body': 'OK'}


def send_ifttt_alert(alert, gas_val, flame):
    if 'FIRE' in alert.upper() and 'GAS' in alert.upper():
        value1 = "FIRE + GAS ALERT"
    elif 'FIRE' in alert.upper():
        value1 = "FIRE DETECTED"
    elif 'GAS' in alert.upper():
        value1 = "GAS LEAKAGE DETECTED"
    else:
        value1 = alert if alert else "DANGER ALERT"

    timestamp = datetime.now(timezone.utc).strftime('%H:%M UTC')
    payload = json.dumps({
        "value1": value1,
        "value2": f"Gas Level: {gas_val} (Danger: 650+)",
        "value3": f"Flame: {'YES - FIRE!' if flame == 0 else 'No'} | Time: {timestamp}"
    }).encode('utf-8')

    url = f"https://maker.ifttt.com/trigger/{IFTTT_ALERT}/with/key/{IFTTT_KEY}"
    req = urllib.request.Request(url, data=payload, headers={'Content-Type': 'application/json'})
    try:
        urllib.request.urlopen(req)
        print("✓ IFTTT alert sent!")
    except Exception as e:
        print("✗ IFTTT alert failed:", str(e))


def send_ifttt_allclear(gas_val):
    timestamp = datetime.now(timezone.utc).strftime('%H:%M UTC')
    payload = json.dumps({
        "value1": "ALL CLEAR - System Safe",
        "value2": f"Gas Level normal: {gas_val}",
        "value3": f"No flame detected | Time: {timestamp}"
    }).encode('utf-8')

    url = f"https://maker.ifttt.com/trigger/{IFTTT_CLEAR}/with/key/{IFTTT_KEY}"
    req = urllib.request.Request(url, data=payload, headers={'Content-Type': 'application/json'})
    try:
        urllib.request.urlopen(req)
        print("✓ IFTTT All Clear sent!")
    except Exception as e:
        print("✗ IFTTT All Clear failed:", str(e))