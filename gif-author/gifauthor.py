
from gi.repository import Gtk, Gdk, GdkPixbuf
import cv2, numpy as np, array

class Application():

	def __init__(self):

		self.window = Gtk.Window(title='Gif Author')
		#self.window.set_default_size(0.5 * Gdk.Screen.width(),
		#														 0.5 * Gdk.Screen.height())

		self.create_initial_widgets()

		self.window.connect('delete-event', Gtk.main_quit)
		self.window.show_all()
		Gtk.main()

	def create_initial_widgets(self):
		
		self.grid = Gtk.Grid.new()
		self.grid.set_property('margin', 10)
		self.grid.set_column_spacing(10)
		self.grid.set_row_spacing(10)
		

		self.file_open_lbl = Gtk.Label('Select Source Video:')
		self.file_open_btn = Gtk.FileChooserButton.new('Select Source Video',
																									 Gtk.FileChooserAction.OPEN)
		self.file_open_btn.connect('file-set', self.on_file_select)

		self.image = Gtk.Image.new()
		self.image.set_size_request(0.5*Gdk.Screen.width(),
																0.5*Gdk.Screen.height())

		self.grid.attach(self.file_open_lbl, 1,0,1,1)
		self.grid.attach(self.file_open_btn, 2,0,1,1)
		self.grid.attach(self.image, 0,1,4,1)

		self.window.add(self.grid)

		self.on_file_select(self.file_open_btn)


	def on_file_select(self, widget):
		#cap = cv2.VideoCapture(widget.get_filename())
		cap = cv2.VideoCapture('/home/hjk/Workspace/gif-author/TestVideo.mp4')

		ret, img = cap.read()

		print("{}x{} {} FPS".format(
			cap.get(cv2.CAP_PROP_FRAME_HEIGHT),
			cap.get(cv2.CAP_PROP_FRAME_WIDTH),
			cap.get(cv2.CAP_PROP_FPS)))

		print(ret)
		print(type(img))

		#img_a = array.array('B', img.tostring())
		
		#data, colorspace, has_alpha, bits_per_sample, width, height, rowstride, destroy_fn, *destroy_fn_data
		pb = GdkPixbuf.Pixbuf.new( 
																				GdkPixbuf.Colorspace.RGB, 
																				False, 
																				8, 
																				img.shape[1], 
																				img.shape[0])
		pb.fill(0xeeff2dff)
		pixels = pb.get_pixels()
		print(type(pixels))
		print(pixels[0:10])
		pixels = img.tostring()
		print(pb.get_pixels()[0:10])
		self.image.set_from_pixbuf(pb)


if __name__ == '__main__':
	app = Application()
