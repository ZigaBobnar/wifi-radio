import { WavServer } from "./wavserver";
import { defaultPlaylist, port } from "./config";

const server = new WavServer(port, defaultPlaylist);
server.startServer();
