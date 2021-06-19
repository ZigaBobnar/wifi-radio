import { Playlist, PlaylistItem } from "./Playlist";
import { StreamListener } from "./StreamListener";

/**
 * Streamer implementation.
 */
class Streamer {
  playlist: Playlist;
  listeners: StreamListener[] = [];

  constructor(
    playlistItems: string[],
    public chunkSize = 20000,
    public sampleRate = 20000
  ) {
    this.playlist = new Playlist(
      playlistItems,
      this.sampleRate,
      this.chunkSize
    );
  }

  startPlaylist() {
    this.playNextTrackFromPlaylist();
    this.sendNextChunk();
  }

  getCurrentlyPlayingDetails() {
    if (!this.playlist.current) {
      return {
        listeners: this.listeners.map((l) => l.name),
        tracks: this.playlist.tracks,
      };
    }

    return {
      track: {
        path: this.playlist.current.track.path,
        percentage:
          Math.round(
            (10000 * this.playlist.current.currentChunk) /
              this.playlist.current.track.chunkCount
          ) / 100,
        totalLengthMs: this.playlist.current.track.totalLengthMs,
        samplingFrequency: this.playlist.current.track.sampleRate,
        currentChunk: this.playlist.current.currentChunk,
        totalChunks: this.playlist.current.track.chunkCount,
      },
      listeners: this.listeners.map((l) => l.name),
      tracks: this.playlist.tracks,
    };
  }

  getTrackDetails(trackIndex: number) {
    const item = this.playlist.getItem(trackIndex);
    if (item == null) return null;

    return {
      path: item.track.path,
      totalLengthMs: item.track.totalLengthMs,
      samplingFrequency: item.track.sampleRate,
      totalChunks: item.track.chunkCount,
    };
  }

  playNextTrackFromPlaylist() {
    this.playlist.playNext();
  }

  playPreviousTrackFromPlaylist() {
    this.playlist.playPrevious();
  }

  sendNextChunk() {
    const item = this.playlist.current;
    if (!item) {
      return;
    }

    const chunkIndex = item.playNextChunk();
    if (chunkIndex == -1) {
      console.log("Finished current track");
      this.playNextTrackFromPlaylist();
      this.sendNextChunk();
      return;
    }

    /*console.log(
      `Streamer chunk [${
        Math.round((10000 * chunkIndex) / item.track.chunkCount) / 100
      }%]`
    );*/

    const chunk = item.track.getChunk(chunkIndex);

    for (let listener of this.listeners) {
      listener.writeFn(chunk);
    }

    setTimeout(() => {
      this.sendNextChunk();
    }, (990 * this.chunkSize) / this.sampleRate);
  }

  attachListener(listener: StreamListener) {
    console.log(`Attaching listener ${listener.name}`);

    this.listeners.push(listener);
  }

  detachListener(listener: StreamListener) {
    console.log(`Detaching listener ${listener.name}`);

    this.listeners = this.listeners.filter((l) => l.name != listener.name);
  }
}

export { Streamer };
