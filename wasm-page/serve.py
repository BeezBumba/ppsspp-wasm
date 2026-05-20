#!/usr/bin/env python3
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
import argparse
import ssl
import os
import subprocess
from functools import partial


class WasmThreadingHandler(SimpleHTTPRequestHandler):
    """
    Serves the project root so that /build-wasm/ and /wasm-page/ are both reachable.
    Requests for / or /index.html are transparently rewritten to /wasm-page/index.html.
    Static assets (icons, sw.js, etc.) live in wasm-page/ and are served from there.
    """
    def do_GET(self):
        # Rewrite bare root to the wasm-page index
        if self.path in ("/", "/index.html"):
            self.path = "/wasm-page/index.html"
        elif self.path.split("?")[0] in ("/favicon.ico", "/manifest.webmanifest", "/sw.js"):
            self.path = "/wasm-page" + self.path
        elif self.path.startswith("/icons/"):
            self.path = "/wasm-page" + self.path
        elif self.path.split("?")[0] == "/assets-manifest.txt":
            self.path = "/wasm-page/assets-manifest.txt"
        super().do_GET()

    def end_headers(self):
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        self.send_header("Cross-Origin-Resource-Policy", "same-origin")
        super().end_headers()


def generate_self_signed_cert(cert_file, key_file):
    """Generate a self-signed certificate using openssl."""
    subprocess.run([
        "openssl", "req", "-x509", "-newkey", "rsa:2048",
        "-keyout", key_file, "-out", cert_file,
        "-days", "365", "-nodes",
        "-subj", "/CN=localhost"
    ], check=True)
    print(f"Self-signed certificate generated: {cert_file}, {key_file}")


def main():
    parser = argparse.ArgumentParser(description="Serve PPSSPP wasm with pthread-compatible headers.")
    parser.add_argument("--bind", "--address", dest="bind", default="192.168.1.170")
    parser.add_argument("--port", type=int, default=8081)
    parser.add_argument("--https", action="store_true", help="Enable HTTPS (TLS)")
    parser.add_argument("--cert", default="cert.pem", help="TLS certificate file (PEM)")
    parser.add_argument("--key", default="key.pem", help="TLS private key file (PEM)")
    parser.add_argument("--dir", default=None, help="Directory to serve (default: project root, parent of this script)")
    args = parser.parse_args()

    # Serve from the project root (parent of wasm-page/), so build-wasm/ is accessible
    script_dir = os.path.dirname(os.path.abspath(__file__))
    serve_dir = args.dir or os.path.dirname(script_dir)
    handler = partial(WasmThreadingHandler, directory=serve_dir)

    server = ThreadingHTTPServer((args.bind, args.port), handler)

    if args.https:
        if not os.path.exists(args.cert) or not os.path.exists(args.key):
            generate_self_signed_cert(args.cert, args.key)
        ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        ctx.load_cert_chain(certfile=args.cert, keyfile=args.key)
        server.socket = ctx.wrap_socket(server.socket, server_side=True)
        scheme = "https"
    else:
        scheme = "http"

    print(f"Serving on {scheme}://{args.bind}:{args.port}/  (from {serve_dir})")
    server.serve_forever()


if __name__ == "__main__":
    main()
