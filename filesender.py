from flask import Flask, send_file
app = Flask(__name__)


@app.route('/')
def index():
    return "<html><h3>実習資料</h3><a href='/export'>ダウンロード</a></html>"


@app.route("/export")
def export_action():
    filename = "送りたいファイルのパス"
    return send_file(
        path=filename,
        as_attachment=True
    )


if __name__ == "__main__":
    print(app.url_map)
    app.run(debug=True,host='0.0.0.0', port=80)
