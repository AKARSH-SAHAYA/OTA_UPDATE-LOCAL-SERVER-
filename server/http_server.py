from flask import Flask, send_file, abort
import os

app = Flask(__name__)

@app.route("/firmware", methods=["GET"])
def firmware():
    firmware_path = "firmware.bin"
    if os.path.exists(firmware_path):
        return send_file(firmware_path, mimetype="application/octet-stream")
    else:
        return abort(404, description="firmware.bin not found")

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
