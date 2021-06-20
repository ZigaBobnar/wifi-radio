import { WavServer } from "./WavServer";
import { serverConfig } from "./config";

(async () => {
  const server = new WavServer(serverConfig);

  await server.startServer();
})();
