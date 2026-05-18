#!/usr/bin/env python3
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
import argparse


class WasmThreadingHandler(SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        self.send_header("Cross-Origin-Resource-Policy", "same-origin")
        super().end_headers()


def main():
    parser = argparse.ArgumentParser(description="Serve PPSSPP wasm with pthread-compatible headers.")
    parser.add_argument("--bind", "--address", dest="bind", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8081)
    args = parser.parse_args()

    server = ThreadingHTTPServer((args.bind, args.port), WasmThreadingHandler)
    print(f"Serving on http://{args.bind}:{args.port}/wasm-page/")
    server.serve_forever()


if __name__ == "__main__":
    main()
