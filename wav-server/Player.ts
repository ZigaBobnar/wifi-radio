import { Track } from "./Track";
import { TrackDefinition } from "./TrackDefinition";

class PlayerClient {
  constructor(public trackId: number, public chunkId: number) {}
}

/**
 * Simple audio streamer.
 */
class Player {
  tracks: Track[] = [];
  clients: { [key: string]: PlayerClient } = {};

  constructor(
    tracksDefinitions: TrackDefinition[],
    public readonly chunkSize: number,
    public readonly sampleRate: number
  ) {
    for (const def of tracksDefinitions) {
      this.tracks.push(new Track(def, chunkSize, sampleRate));
    }
  }

  async init(loadFirst = true) {
    if (loadFirst) {
      await this.tracks[0]?.load();
    }
  }

  async loadMedia() {
    for (const track of this.tracks) {
      await track.load();
    }
  }

  async moveForward(clientId: string) {
    const client = await this.getClient(clientId);

    client.trackId++;
    if (client.trackId >= this.tracks.length) {
      client.trackId = 0;
    }

    client.chunkId = -1;
  }

  async moveBackward(clientId: string) {
    const client = await this.getClient(clientId);

    client.trackId--;
    if (client.trackId == 0) {
      client.trackId = this.tracks.length - 1;
    }

    client.chunkId = -1;
  }

  async getNextChunk(clientId: string) {
    const client = await this.getClient(clientId);

    if (client.trackId >= this.tracks.length) {
      client.trackId = 0;
    }
    let track = this.tracks[client.trackId];
    await track.load();

    client.chunkId++;
    if (client.chunkId >= track.chunkCount) {
      client.trackId++;
      if (client.trackId >= this.tracks.length) {
        client.trackId = 0;
      }
      track = this.tracks[client.trackId];
      await track.load();

      client.chunkId = 0;
    }

    const chunk = await track.getChunk(client.chunkId);
    if (!chunk || chunk.length == 0) {
      return null;
    }

    return {
      trackId: client.trackId,
      chunkId: client.chunkId,
      data: chunk,
    };
  }

  async getCurrentTrackInfo(clientId: string) {}

  async getTrackInfo(trackId: number) {}

  async getPlayerPlaybackDetails() {
    return {
      tracks: this.tracks,
      clients: this.clients,
    }
  }

  private async getClient(clientId: string) {
    if (!this.clients[clientId]) {
      console.log(`New client registered ${clientId}`);

      this.clients[clientId] = {
        trackId: 0,
        chunkId: -1,
      };
    }

    return this.clients[clientId];
  }
}

export { Player };
