import { Track } from "./Track";

/**
 * Playlist item
 */
class PlaylistItem {
  currentChunk = -1;

  constructor(public track: Track) {}

  playNextChunk() {
    this.currentChunk++;

    if (!this.track.isLoaded) {
      this.track.loadTrack();
    }

    if (this.currentChunk >= this.track.chunkCount) {
      this.currentChunk = -1;
    }

    return this.currentChunk;
  }
}

/**
 * Playlist container that holds tracks data, media and current track info.
 */
class Playlist {
  private items: PlaylistItem[] = [];
  private currentIndex: number = -1;
  private currentItem: PlaylistItem | null = null;

  constructor(
    paths: string[],
    sampleRate: number,
    chunkSize: number,
    deferLoading = true
  ) {
    for (let path of paths) {
      this.addItem(path, sampleRate, chunkSize, deferLoading);
    }
  }

  get current() {
    return this.currentItem;
  }

  get tracks() {
    return this.items.map(i => i.track);
  }

  getItem(index: number): PlaylistItem | null {
    if (index >= this.items.length) return null;

    return this.items[index];
  }

  addItem(
    path: string,
    sampleRate: number,
    chunkSize: number,
    deferLoading = true
  ) {
    this.items.push(
      new PlaylistItem(new Track(path, sampleRate, chunkSize, deferLoading))
    );
  }

  playNext() {
    if (this.items.length == 0) {
      console.info("No tracks in playlist!");
      return null;
    }

    if (this.current) {
      this.current.currentChunk = -1;
    }

    this.currentIndex++;

    if (this.currentIndex >= this.items.length) {
      this.currentIndex = 0;
    }

    this.currentItem = this.items[this.currentIndex];
    console.info(`Playing track ${this.currentItem.track.path}`)

    return this.currentItem;
  }

  playPrevious() {
    if (this.items.length == 0) {
      console.info("No tracks in playlist!");
      return;
    }

    if (this.current) {
      this.current.currentChunk = -1;
    }

    if (this.currentIndex == 0 || this.currentIndex >= this.items.length) {
      this.currentIndex = this.items.length - 1;
    } else {
      this.currentIndex--;
    }
    
    this.currentItem = this.items[this.currentIndex];
    console.info(`Playing track ${this.currentItem.track.path}`)

    return this.currentItem;
  }
}

export { Playlist, PlaylistItem };
